/*
现在我们给 globalmem 增加这样的约束：把 globalmem 中的全局内存变成一个 FIFO，
只有当 FIFO 中有数据的时候（即有进程把数据写到这个 FIFO 而且没有被读进程读空），
读进程才能把数据读出，而且读取后的数据会从 globalmem 的全局内存中被拿掉；
只有当 FIFO 不是满的时（即还有一些空间未被写，或写满后被读进程从这个 FIFO
 中读出了数据），写进程才能往这个 FIFO 中写入数据。

现在，将 globalmem 重命名为 "globalfifo"。在 globalfifo 中，
读 FIFO 将唤醒写 FIFO 的进程（如果之前 FIFO 正好是满的），而写 FIFO 
也将唤醒读 FIFO 的进程（如果之前 FIFO 正好是空的）
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>

#define GLOBALFIFO_SIZE	0x1000
#define FIFO_CLEAR 0x1
#define GLOBALFIFO_MAJOR 231

static int globalfifo_major = GLOBALFIFO_MAJOR;
module_param(globalfifo_major, int, S_IRUGO);

struct globalfifo_dev {
	struct cdev cdev;					// 设备结构体
	unsigned int current_len;			// 当前 FIFO 中有效数据的长度
	unsigned char mem[GLOBALFIFO_SIZE];	// 设备空间
	struct mutex mutex;					//	互斥锁
	wait_queue_head_t r_wait;			//	等待队列头节点
	wait_queue_head_t w_wait;			//
};

struct globalfifo_dev *globalfifo_devp;

static int globalfifo_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalfifo_devp;
	return 0;
}

static int globalfifo_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static long globalfifo_ioctl(struct file *filp, unsigned int cmd,
			     unsigned long arg)
{
	struct globalfifo_dev *dev = filp->private_data;

	switch (cmd) {
	case FIFO_CLEAR:
		mutex_lock(&dev->mutex);
		dev->current_len = 0;
		memset(dev->mem, 0, GLOBALFIFO_SIZE);
		mutex_unlock(&dev->mutex);

		printk(KERN_INFO "globalfifo is set to zero\n");
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

static unsigned int globalfifo_poll(struct file *filp, poll_table * wait)
{
	unsigned int mask = 0;
	struct globalfifo_dev *dev = filp->private_data;

	mutex_lock(&dev->mutex);

	poll_wait(filp, &dev->r_wait, wait);
	poll_wait(filp, &dev->w_wait, wait);

	if (dev->current_len != 0) {
		mask |= POLLIN | POLLRDNORM;
	}

	if (dev->current_len != GLOBALFIFO_SIZE) {
		mask |= POLLOUT | POLLWRNORM;
	}

	mutex_unlock(&dev->mutex);
	return mask;
}


static ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    int ret;
    struct globalfifo_dev *dev = filp->private_data;
	// 创建一个等待队列元素（wait_queue），并将其与特定的任务（task）关联起来。
	// 等待队列元素通常用于进程等待某种条件的发生，然后在条件满足时唤醒等待的进程。
    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex); // 获取互斥锁以保护共享数据
    add_wait_queue(&dev->r_wait, &wait); // 将当前进程添加到等待队列

    // 当设备空间为空，根据文件标志判断是否阻塞
    // 非阻塞模式返回 -EAGAIN，否则标记进程为TASK_INTERRUPTIBLE并休眠
    while (dev->current_len == 0) {
        if (filp->f_flags & O_NONBLOCK) {
            ret = -EAGAIN; // 非阻塞模式，立即返回
            goto out;
        }
        __set_current_state(TASK_INTERRUPTIBLE);
        mutex_unlock(&dev->mutex); // 解锁以允许其他进程运行

        // 休眠当前进程，等待数据可用或信号中断
        schedule();

        if (signal_pending(current)) {
            ret = -ERESTARTSYS; // 如果是由于信号唤醒，则返回 -ERESTARTSYS
            goto out2;
        }

        mutex_lock(&dev->mutex); // 重新获取互斥锁以检查数据是否可用
    }

    if (count > dev->current_len)
        count = dev->current_len;

    if (copy_to_user(buf, dev->mem, count)) {
        ret = -EFAULT; // 如果复制到用户空间失败，返回错误
        goto out;
    } else {
        // 移动数据和更新当前长度
        memcpy(dev->mem, dev->mem + count, dev->current_len - count);
        dev->current_len -= count;
        printk(KERN_INFO "read %d bytes(s), current_len:%d\n", count, dev->current_len);

        wake_up_interruptible(&dev->w_wait); // 唤醒等待写的进程
        ret = count;
    }

out:
    mutex_unlock(&dev->mutex); // 解锁互斥锁
out2:
    remove_wait_queue(&dev->r_wait, &wait); // 从等待队列中移除当前进程
    set_current_state(TASK_RUNNING); // 设置进程状态为TASK_RUNNING
    return ret;
}



static ssize_t globalfifo_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct globalfifo_dev *dev = filp->private_data;
	int ret;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->w_wait, &wait);

	// 当设备空间满时，设备文件为阻塞模式，解锁，返回
	// 为非阻塞模式，标记浅度睡眠，解锁
	while (dev->current_len == GLOBALFIFO_SIZE) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);

		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if (count > GLOBALFIFO_SIZE - dev->current_len)
		count = GLOBALFIFO_SIZE - dev->current_len;

	if (copy_from_user(dev->mem + dev->current_len, buf, count)) {
		ret = -EFAULT;
		goto out;
	} else {
		dev->current_len += count;
		printk(KERN_INFO "written %d bytes(s),current_len:%d\n", count,
		       dev->current_len);

		wake_up_interruptible(&dev->r_wait);

		ret = count;
	}

 out:
	mutex_unlock(&dev->mutex);
 out2:
	remove_wait_queue(&dev->w_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

static const struct file_operations globalfifo_fops = {
	.owner = THIS_MODULE,
	.read = globalfifo_read,
	.write = globalfifo_write,
	.unlocked_ioctl = globalfifo_ioctl,
	.poll = globalfifo_poll,
	.open = globalfifo_open,
	.release = globalfifo_release,
};

static void globalfifo_setup_cdev(struct globalfifo_dev *dev, int index)
{
	int err, devno = MKDEV(globalfifo_major, index);

	cdev_init(&dev->cdev, &globalfifo_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding globalfifo%d", err, index);
}

static int __init globalfifo_init(void)
{
	int ret;
	dev_t devno = MKDEV(globalfifo_major, 0);

	if (globalfifo_major)
		ret = register_chrdev_region(devno, 1, "globalfifo");
	else {
		ret = alloc_chrdev_region(&devno, 0, 1, "globalfifo");
		globalfifo_major = MAJOR(devno);
	}
	if (ret < 0)
		return ret;

	globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev), GFP_KERNEL);
	if (!globalfifo_devp) {
		ret = -ENOMEM;
		goto fail_malloc;
	}

	globalfifo_setup_cdev(globalfifo_devp, 0);

	
	mutex_init(&globalfifo_devp->mutex); // 初始化 互斥锁
	init_waitqueue_head(&globalfifo_devp->r_wait);  // 初始化 等待队列头
	init_waitqueue_head(&globalfifo_devp->w_wait);  // 初始化 等待队列头

	return 0;

fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}
module_init(globalfifo_init);

static void __exit globalfifo_exit(void)
{
	cdev_del(&globalfifo_devp->cdev);
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1);
}
module_exit(globalfifo_exit);

MODULE_AUTHOR("Barry Song <baohua@kernel.org>");
MODULE_LICENSE("GPL v2");


/*
 sudo insmod globalfifo.ko
 sudo mknod /dev/globalfifo c 231 0
cat /dev/globalfifo &
sudo su
echo 'hello' > /dev/globalfifo
root@baohua-VirtualBox:/home/baohua/develop/training/kernel/drivers/globalfifo/ch8# echo 'hello' > /dev/globalfifo
root@baohua-VirtualBox:/home/baohua/develop/training/kernel/drivers/globalfifo/ch8# hello




*/