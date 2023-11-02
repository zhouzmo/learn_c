


#### 删除指定文件
find . -type f -name "*.out" -exec rm {} \;
find . -type f -name "*.exe" -exec rm {} \;


# 开启内核打印
echo "7	4	1	7" > /proc/sys/kernel/printk
# 屏蔽内核打印
echo 1       4       1      7 > /proc/sys/kernel/printk