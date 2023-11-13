#include "asm/cacheflush.h"
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


static struct class *hello_class;
static int major;
static unsigned char hello_buf[100];

static int hello_open (struct inode *node, struct file *filp)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t hello_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
    unsigned long len = size > 100 ? 100 : size;

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /*
    `copy_to_user` 函数是Linux内核编程中的一个函数，用于将数据从内核空间复制到用户空间。
    这个函数的目的是将内核中的数据传递给用户空间的缓冲区，通常在驱动程序或内核模块中使用。

    unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);

    - `to`: 这是用户空间的目标缓冲区，表示要将数据复制到哪里。通常是用户空间中的一个缓冲区指针。
    - `from`: 这是内核空间中的源数据，表示要复制的数据的来源。通常是内核空间中的一个数据指针。
    - `n`: 这是要复制的字节数，表示要从源数据中复制多少字节到目标缓冲区。

    - 如果复制成功，`copy_to_user` 返回0。
    - 如果复制过程中发生错误，返回一个负数，通常是一个负的错误代码。

    `copy_to_user` 的主要作用是确保在内核空间和用户空间之间进行数据传输时
    不会破坏用户空间的完整性。它会检查用户空间缓冲区的合法性，
    以防止内核破坏用户空间内存，从而提高系统的稳定性和安全性。

    */
    copy_to_user(buf, hello_buf, len);

    return len;
}

static ssize_t hello_write(struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
    unsigned long len = size > 100 ? 100 : size;

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /*
     copy_from_user

    unsigned long copy_from_user(void *to, const void __user *from, unsigned long n)

    - to：目标内存地址，用于存储从用户空间复制的数据
    - from：源内存地址，指向用户空间的数据
    - n：要复制的字节数

    返回值作用：
    该函数用于将数据从用户空间复制到内核空间。它将从用户空间的源地址（from）
    复制n个字节的数据到内核空间的目标地址（to）。
    如果成功复制所有数据，则返回0；如果复制过程中出现错误，则返回未能复制的字节数。
    */
    copy_from_user(hello_buf, buf, len);

    return len;
}

static int hello_release (struct inode *node, struct file *filp)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

/*=== 1. 构建 file_operations 结构体 ===*/
static const struct file_operations hello_drv = {
    .owner      = THIS_MODULE,
	.read		= hello_read,
	.write		= hello_write,
	.open		= hello_open,
    .release    = hello_release,
};


/*=== 2. register_chrdev/unregister_chrdev 注册/卸载字符设备驱动程序  ===*/
/*=== 3. 实现入口/出口函数 ，创建/销毁类，创建/销毁设备===*/
static int hello_init(void)
{
    major = register_chrdev(0, "100ask_hello", &hello_drv);

    // 创建虚拟文件 /sys/class/hello_class/
    /*
    这是一个用于创建字符设备驱动类的函数调用，通常在 Linux 内核编程中使用。

    struct class *class_create(struct module *owner, const char *name);

    1. `owner`：这是指向模块结构体的指针，通常使用 `THIS_MODULE` 宏表示当前的内核模块。它表示将拥有该类的内核模块。
    2. `name`：这是一个用于标识创建的类的名称，通常是一个字符串。在您的示例中，类的名称是 "hello_class"。

    - 该函数返回一个指向 `struct class` 结构体的指针，表示创建的字符设备驱动类。这个类将用于将字符设备与用户空间的设备节点进行关联，以便用户空间应用程序可以与设备进行交互。

    */
	hello_class = class_create(THIS_MODULE, "hello_class");
	if (IS_ERR(hello_class)) {
		printk("failed to allocate class\n");
		return PTR_ERR(hello_class);
	}

    // 创建设备节点  /dev/hello
    /*
    这是一个用于创建字符设备节点的函数调用，通常在 Linux 内核编程中使用。以下是该函数的概要以及参数详解：

    struct device *device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...);

    1. `class`：这是一个指向之前创建的字符设备驱动类（使用 `class_create` 创建）的指针，用于指定要创建设备节点的类。
    2. `parent`：这是指向父设备的指针，通常为 `NULL`。如果在设备树中有父设备的关系，可以使用它。
    3. `devt`：这是设备号（主设备号和次设备号），通常使用 `MKDEV(major, minor)` 宏来创建。在您的示例中，`major` 是主设备号，0 是次设备号，表示设备号。
    4. `drvdata`：这是一个指向设备驱动特定数据的指针，通常为 `NULL`。
    5. `fmt`：这是一个格式化字符串，用于生成设备节点的名称。在您的示例中，名称为 "hello"。

    - 该函数返回一个指向 `struct device` 结构体的指针，表示创建的设备节点。这个设备节点将与字符设备驱动类关联，以便用户空间应用程序可以与设备进行交互。

    在您的示例中，`device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello")` 的作用是创建一个设备节点，关联到之前创建的字符设备驱动类 `hello_class`，设备号为 `major` 主设备号和 0 次设备号，并命名为 "hello"。这个设备节点将用于用户空间应用程序与字符设备的通信。
    */
    device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello");  /* /dev/hello */

   return 0;
}


/*=== 4. 声明入口函数，出口函数，开源协议 ===*/
static void hello_exit(void)
{
    device_destroy(hello_class, MKDEV(major, 0));

    class_destroy(hello_class);

    unregister_chrdev(major, "100ask_hello");
}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
