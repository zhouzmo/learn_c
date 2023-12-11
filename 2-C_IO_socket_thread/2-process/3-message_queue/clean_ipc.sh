#!/bin/bash


ipcs -q | awk '/^0x/ {system("ipcrm -q " $2)}'
ipcs -m | awk '/^0x/ {system("ipcrm -m " $2)}'
ipcs -s | awk '/^0x/ {system("ipcrm -s " $2)}'
echo "clean success"

# 检查参数数量和有效性
if [ "$#" -ne 2 ]; then
    echo "usage: $0 user shm|sem|msg|all"
    # exit 1
    elif [ "$2" != "shm" ] && [ "$2" != "sem" ] && [ "$2" != "msg" ] && [ "$2" != "all" ]; then
    echo "usage: $0 user shm|sem|msg|all"
    # exit 1
fi

# 删除共享内存段
if [ "$2" = "shm" ] || [ "$2" = "all" ]; then
    for i in $(ipcs -m | awk '/^0x/{print $2}')
    do
        ipcrm -m $i
        echo -e "Removed Shared Memory Segment: $i"
    done
fi

# 删除信号量数组
if [ "$2" = "sem" ] || [ "$2" = "all" ]; then
    for i in $(ipcs -s | awk '/^0x/{print $2}')
    do
        ipcrm -s $i
        echo -e "Removed Semaphore Array: $i"
    done
fi

# 删除消息队列
if [ "$2" = "msg" ] || [ "$2" = "all" ]; then
    for i in $(ipcs -q | awk '/^0x/{print $2}')
    do
        ipcrm -q $i
        echo -e "Removed Message Queue: $i"
    done
fi