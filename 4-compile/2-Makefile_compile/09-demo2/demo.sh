#!/bin/bash
SD_PATH=/tmp/mnt/sdcard
UPDATE_PATH=/update/kernel

if [ -f ${UPDATE_PATH} ]; then
    cp ${SD_PATH}/uImage.bin ${UPDATE_PATH}/
else
    mkdir ${UPDATE_PATH}/
    cp ${SD_PATH}/uImage.bin ${UPDATE_PATH}/
fi

#uImage md5校验
md5sum ${UPDATE_PATH}/uImage.bin > ota.txt.md5
cat ${UPDATE_PATH}/ota.txt.md5
file_ota=$(cat ota.txt.md5)

#备份原分区
echo /dev/mtd2 > ${UPDATE_PATH}/original_uImage.bin

echo "loading..."
flash_eraseall /dev/mtd2
flashcp  ${UPDATE_PATH}/uImage.bin /dev/mtd2

md5sum /dev/mtd2 > ${UPDATE_PATH}/mtd2.txt.md5
cat ${UPDATE_PATH}/mtd2.txt.md5
file_mtd2=$(cat mtd2.txt.md5)


if [ "$file_ota" -eq "$file_mtd2" ]; then
    echo "update success"
else
    echo "unsuccess uImage..."
    flash_eraseall /dev/mtd2
    flashcp  ${UPDATE_PATH}/original_uImage.bin /dev/mtd2
    echo "update fail"
fi

reboot