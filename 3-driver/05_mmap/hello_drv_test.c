
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * ./hello_drv_test
 */
int main(int argc, char **argv)
{
	int fd;
	char *buf;
	int len;
	char str[1024];
	
	
	/* 1. 打开文件 */
	fd = open("/dev/hello", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file /dev/hello\n");
		return -1;
	}

	/* 2. mmap 
	 * MAP_SHARED  : 多个APP都调用mmap映射同一块内存时, 对内存的修改大家都可以看到。
	 *               就是说多个APP、驱动程序实际上访问的都是同一块内存
	 * MAP_PRIVATE : 创建一个copy on write的私有映射。
	 *               当APP对该内存进行修改时，其他程序是看不到这些修改的。
	 *               就是当APP写内存时, 内核会先创建一个拷贝给这个APP, 
	 *               这个拷贝是这个APP私有的, 其他APP、驱动无法访问。
	 */
	buf =  mmap(NULL, 1024*8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// buf =  mmap(NULL, 1024*8, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED)
	{
		printf("can not mmap file /dev/hello\n");
		return -1;
	}
	
	printf("mmap address = 0x%x\n", buf);
	printf("buf origin data = %s\n", buf); /* old */

	/* 3. write */
	strcpy(buf, "app_new");
	printf("strcpy end  buf = %s\n", buf); 
	printf("strcpy end  str = %s\n", buf); 
	/* 4. read & compare */
	/* 对于MAP_SHARED映射:  str = "new" 
	 * 对于MAP_PRIVATE映射: str = "old" 
	 */
	read(fd, str, 1024);  
	printf("read(fd, str, 1024) end  buf = %s\n", buf); 
	printf("read(fd, str, 1024) end  str = %s\n", str); 
	if (strcmp(buf, str) == 0)
	{
		/* 对于MAP_SHARED映射，APP写的数据驱动可见
		 * APP和驱动访问的是同一个内存块
		 */
		printf("compare ok!\n");
	}
	else
	{
		/* 对于MAP_PRIVATE映射，APP写数据时, 是写入原来内存块的"拷贝"
		 */
		printf("compare err!\n");
		printf("str = %s!\n", str);  /* old */
		printf("buf = %s!\n", buf);  /* new */
	}

	while (1)
	{
		sleep(10);  /* cat /proc/pid/maps */
	}
	
	munmap(buf, 1024*8);
	close(fd);
	
	return 0;
}


/*

# MAP_SHARED 共享

rmmod hello_drv
insmod hello_drv.ko
 


 ./hello_drv_test
[52243.340134] /home/book/learn_c/3-driver/05_mmap/hello_drv.c hello_drv_open line 66
mmap address = 0x76f09000
[52243.352749] /home/book/learn_c/3-driver/05_mmap/hello_drv.c hello_drv_read line 33

buf origin data = driver_full_data
strcpy end  buf = app_new
strcpy end  str = app_new
read(fd, str, 1024) end  buf = app_new

# SHARE 模式下，改变 buf 值，会导致 fd被改

read(fd, str, 1024) end  str = app_new
compare ok!
*/


/*
# MAP_PRIVATE 

rmmod hello_drv
insmod hello_drv.ko



./hello_drv_test
[52025.519162] /home/book/learn_c/3-driver/05_mmap/hello_drv.c hello_drv_open line 66
mmap address = 0x76fa4000
[52025.529900] /home/book/learn_c/3-driver/05_mmap/hello_drv.c hello_drv_read line 33

buf origin data = driver_full_data
strcpy end  buf = app_new
strcpy end  str = app_new
read(fd, str, 1024) end  buf = app_new

# private 模式下，改变 buf 值，不会导致 fd被改，使用 copy_on_write 技术

read(fd, str, 1024) end  str = driver_full_data
compare err!
str = driver_full_data!
buf = app_new!




*/