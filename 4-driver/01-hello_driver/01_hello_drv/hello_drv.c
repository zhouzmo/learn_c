#include <linux/mm.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/raw.h>
#include <linux/tty.h>
#include <linux/capability.h>
#include <linux/ptrace.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/backing-dev.h>
#include <linux/shmem_fs.h>
#include <linux/splice.h>
#include <linux/pfn.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/uio.h>

#include <linux/uaccess.h>


static int major;

static int hello_open (struct inode *node, struct file *filp)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}
static ssize_t hello_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return size;
}

static ssize_t hello_write(struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return size;
}

static int hello_release (struct inode *node, struct file *filp)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

/*=== 1. 构建 file_operations 结构体 ===*/
/*
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
			u64);
	ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
			u64);
};
*/
static const struct file_operations hello_drv = {
    .owner      = THIS_MODULE,
	.read		= hello_read,
	.write		= hello_write,
	.open		= hello_open,
    .release    = hello_release,
};


/*=== 2. register_chrdev/unregister_chrdev 注册/卸载字符设备驱动程序  ===*/
/*=== 3. 实现入口/出口函数 ===*/
static int hello_init(void)
{
/*
这是一个用于注册字符设备驱动的函数调用，通常在 Linux 内核编程中使用。

int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops);

1. `major`：这是字符设备的主设备号，通常用于标识设备驱动。传递0表示由内核自动分配一个主设备号。如果您有特定需求，可以手动分配一个主设备号。
2. `name`：这是一个用于标识设备的名称，通常是一个字符串。在您的例子中，名称为 "100ask_hello"。
3. `fops`：这是一个指向字符设备驱动操作函数结构体的指针，通常包含了字符设备驱动的操作函数，例如读、写、打开和关闭。这个结构体的定义通常为 `struct file_operations`。

- 该函数返回一个整数值，通常是主设备号。
在成功注册字符设备驱动的情况下，它会返回已分配的主设备号。
如果出现错误，它可能返回负数值，表示注册失败。

*/
   major = register_chrdev(0, "100ask_hello", &hello_drv);
   return 0;
}


static void hello_exit(void)
{
    /*
    这是一个用于注销字符设备驱动的函数调用。以
    void unregister_chrdev(unsigned int major, const char *name);

    1. `major`：这是字符设备的主设备号，用于标识要注销的设备驱动。通常，您需要传递在注册设备驱动时获得的主设备号。
    2. `name`：这是字符设备的名称，用于标识要注销的设备驱动。通常，您需要传递在注册设备驱动时使用的名称。
    - `unregister_chrdev` 函数没有返回值（`void`）。
    它的作用是从内核中注销之前注册的字符设备驱动，以释放相关资源和设备节点。
    */
    unregister_chrdev(major, "100ask_hello");
}

/*=== 4. 声明入口函数，出口函数，开源协议 ===*/
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
