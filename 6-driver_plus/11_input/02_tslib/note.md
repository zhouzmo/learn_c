

### 交叉编译、测试tslib

```sh
tar -xvf tslib-1.21.tar.xz


# 对于IMX6ULL，命令如下
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export PATH=$PATH:/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin

./configure --host=arm-linux-gnueabihf  --prefix=/
make
make install DESTDIR=$PWD/tmp

echo 'main(){}'| arm-linux-gnueabihf-gcc -E -v -

cd tslib-1.21/tmp/
cp include/* /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/../arm-linux-gnueabihf/libc/usr/include
cp -d lib/*so*  /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/../arm-linux-gnueabihf/libc/usr/lib/
```



```
# imx6ull 开发板上
cd /mnt/tslib-1.21/tmp/
cp  lib/*  -drf     /lib
cp  bin/*            /bin
cp  etc/ts.conf  -d  /etc

# 关闭qt命令
mv  /etc/init.d/S99myirhmi2  /root
mv  /etc/init.d/S05lvgl  /root
reboot

ts_test_mt
```

