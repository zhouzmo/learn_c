

# 挂载跟文件系统脚本
#!/bin/bash
mount -t nfs -o nolock,vers=3 192.168.5.11:/home/book /mnt
echo 1       4       1      7 > /proc/sys/kernel/printk




# 删除指定文件
linux 命令行下
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

sudo ifconfig ens38 192.168.5.11



cd /mnt/learn_c/3-driver/0
cd /home/book/learn_c/3-driver/0

# 挂载跟文件系统脚本


# 统计数量
netstat -n|awk '/^tcp/{++S[$NF]}END{for (key in S) print key,S[key]}'  


# 网络
watch -n 0.1 -d  'netstat -na | grep 8888'
watch -n 0.1 -d  'ps -ef | grep -E out'


gdb -tui -q server.out



  printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
  printf("%s:%d\n", __FUNCTION__, __LINE__);