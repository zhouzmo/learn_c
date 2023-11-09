

#### 挂载跟文件系统脚本
#!/bin/bash
mount -t nfs -o nolock,vers=3 192.168.5.11:/home/book /mnt
echo 1       4       1      7 > /proc/sys/kernel/printk




#### 删除指定文件
find . -type f -name "*.out" -exec rm {} \;
find . -type f -name "*.exe" -exec rm {} \;


# 开启内核打印
echo "7	4	1	7" > /proc/sys/kernel/printk
# 屏蔽内核打印
echo 1       4       1      7 > /proc/sys/kernel/printk



# 修改设备树文件
$ vim /home/book/100ask_imx6ull-sdk/Linux-4.9.88/arch/arm/boot/dts/100ask_imx6ull-14x14.dts +101

$ cd /home/book/100ask_imx6ull-sdk/Linux-4.9.88
$ make dtbs



# board
$ cp /mnt/100ask_imx6ull-sdk/Linux-4.9.88/arch/arm/boot/dts/100ask_imx6ull-14x14.dtb /boot/
$ reboot
$ insmod gpio_key_drv.ko
# 按下按键，查看 dmesg
$ dmesg |tail -n20



cd /mnt/learn_c/3-driver/0
cd /home/book/learn_c/3-driver/0