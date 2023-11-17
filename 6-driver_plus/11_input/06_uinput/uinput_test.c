#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/uinput.h>


// 虚拟驱动程序，由app 决定上报事件内容和事件

void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

int main(void)
{
   struct uinput_setup usetup;
   int i;

   int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

   // 对照 F:\SIProject\Linux-4.9.88\drivers\input\misc\uinput.c 进行查看
   /*
    * set propbit，直接操作设备
	*/
   ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

   /*
    * support EV_KEY and which KEY ?   可以产生哪一类事件 设置支持事件类型
	*/
   ioctl(fd, UI_SET_EVBIT, EV_KEY);
   ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
   

   /*
    * support EV_ABS and which axis ? 设置可以产生事件，绝对位移事件
	*/
   ioctl(fd, UI_SET_EVBIT, EV_ABS);
   ioctl(fd, UI_SET_ABSBIT, ABS_X);
   ioctl(fd, UI_SET_ABSBIT, ABS_Y);

   memset(&usetup, 0, sizeof(usetup));
   usetup.id.bustype = BUS_USB;
   usetup.id.vendor = 0x1234; /* sample vendor */
   usetup.id.product = 0x5678; /* sample product */
   strcpy(usetup.name, "Example device");

   /*
    * setup and create 设置创建设备
	*/
   ioctl(fd, UI_DEV_SETUP, &usetup);
   ioctl(fd, UI_DEV_CREATE);

   /*
	* On UI_DEV_CREATE the kernel will create the device node for this
	* device. We are inserting a pause here so that userspace has time
	* to detect, initialize the new device, and can start listening to
	* the event, otherwise it will not notice the event we are about
	* to send. This pause is only needed in our example code!
	*/
   printf("Will sleep 60s, in this time you should run ts_calibreate\n");
   sleep(60);

   /* upper left */
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 100);
   emit(fd, EV_ABS, ABS_Y, 100);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   /* upper right */
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 1000);
   emit(fd, EV_ABS, ABS_Y, 100);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   /* lower right */
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 1000);
   emit(fd, EV_ABS, ABS_Y, 1000);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   /* lower left */
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 100);
   emit(fd, EV_ABS, ABS_Y, 1000);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   /* center */
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 550);
   emit(fd, EV_ABS, ABS_Y, 550);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);


   printf("Will sleep 60s, in this time you should run ts_test\n");
   sleep(60);
   emit(fd, EV_KEY, BTN_TOUCH, 1);
   emit(fd, EV_ABS, ABS_X, 200);
   emit(fd, EV_ABS, ABS_Y, 200);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   i = 200;
   while (i < 1000)
   {
	   emit(fd, EV_ABS, ABS_X, i);
	   emit(fd, EV_ABS, ABS_Y, i);
	   emit(fd, EV_SYN, SYN_REPORT, 0);
	   i += 10;
	   sleep(5);
   }

   emit(fd, EV_KEY, BTN_TOUCH, 0);
   emit(fd, EV_SYN, SYN_REPORT, 0);
   sleep(5);

   ioctl(fd, UI_DEV_DESTROY);
   close(fd);

   return 0;
}



