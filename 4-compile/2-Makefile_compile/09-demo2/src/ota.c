#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

#define UPDATE_PATH "update/"
#define SD_PATH ""

#define MD5_LENGTH 33 // 32(md5 校验码长度) + 1(字符串尾部)
#define CMD_LENGTH_MAX 128

/*

1.检查 指定目录 是否存在 指定文件
2.存在指定文件，对 targetFile 进行 md5 计算
3.备份设备节点下(/dev/mtd2 )的 内容





*/


/*


问题  dev/mtd2 和 image.bin 的关系

这个命令将 sda 盘备份到指定文件 /root/sda.img 中去，其中用到了如下两个选项：

if=文件名：指定输入文件名或者设备名，如果省略“if=文件名”，则表示从标准输入读取。

of=文件名：指定输出文件名或者设备名，如果省略“of=文件名”，则表示写到标准输出。


*/

/* 检查 dirPath 下是否存在 filename*/
int fileExists(char *dirPath, char *filename)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dirPath)) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, filename) == 0)
            {
                closedir(dir);
                return 1; // 文件存在
            }
        }
        closedir(dir);
    }
    return 0; // 文件不存在
}

/* 调用 /usr/bin/md5sum 生成指定文件的 md5 值 */
char *md5sum(char *filePath)
{
    if (filePath == NULL)
    {
        return NULL;
    }

    char cmd[CMD_LENGTH_MAX];
    snprintf(cmd, sizeof(cmd), "/usr/bin/md5sum %s", filePath);

    printf("CMD:%s\n", cmd);

    // 执行命令
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        perror("popen");
        return NULL;
    }

    char *md5_buf = malloc(MD5_LENGTH);
    if (!md5_buf)
    {
        perror("malloc");
        pclose(fp);
        return NULL;
    }

    if (fgets(md5_buf, MD5_LENGTH, fp) == NULL)
    {
        perror("fgets");
        pclose(fp);
        return NULL;
    }

    pclose(fp);

    printf("old md5:%s---sizeof(md5_buf)%ld,strlen(md5_buf)%ld\n", md5_buf, sizeof(md5_buf), strlen(md5_buf));

    return md5_buf;
}

/* todo */
int md5compare(char *md5one, char *md5two)
{
}

int main()
{
    // const char *update_path = UPDATE_PATH;
    // char *sd_path = SD_PATH;
    char *file_name = "111.bin";
    char *md5_old;
    // 计算指定文件的 md5 值
    md5_old = md5sum(file_name);

    printf("old md5:%s\n", md5_old);

    return 0;

    // const char *update_path = UPDATE_PATH;
    // const char *sd_path = SD_PATH;
    // const char *file_name = "111.bin";
    // int ret;

    // char cmd[1024];           // 用于存储 shell 命令
    // char md5_old[MD5_LENGTH]; // 存储 OTA 文件的 MD5 校验值
    // // char md5_new[MD5_LENGTH]; // 存储 MTD2 分区的 MD5 校验值

    // // 构建命令：计算 OTA 文件的 MD5 校验值
    // // /usr/bin/md5sum 111.bin > old.md5
    // snprintf(cmd, sizeof(cmd), "/usr/bin/md5sum %s%s > old.md5", sd_path, file_name);
    // ret = system(cmd);
    // if (ret != 0)
    // {
    //     fprintf(stderr, "errno: %d,info:%s\n", errno, cmd);
    //     return -1;
    // }

    // 读取 OTA 文件的 MD5 校验值
    // FILE *fp_ota = fopen(strcat(update_path, "old.md5"), "r");
    // ret = fscanf(fp_ota, "%s", md5_old);
    // fclose(fp_ota);
    // printf("%s\n", md5_old);

    // 构建命令：备份 MTD2 分区，并计算其 MD5 校验值
    // snprintf(cmd, sizeof(cmd), "echo /dev/mtd2 > %soriginal_uImage.bin && flash_eraseall /dev/mtd2 && flashcp %s%s%s /dev/mtd2 && md5sum /dev/mtd2 > %smtd2.txt.md5", update_path, sd_path, file_name, update_path, update_path);
    // system(cmd);

    // // 读取 MTD2 分区的 MD5 校验值
    // FILE *fp_mtd2 = fopen(strcat(update_path, "mtd2.txt.md5"), "r");
    // fscanf(fp_mtd2, "%s", md5_new);
    // fclose(fp_mtd2);

    // // 比较两个 MD5 校验值是否相同
    // if (strcmp(md5_ota, md5_mtd2) == 0)
    // {
    //     printf("Update success\n");
    // }
    // else
    // {
    //     printf("Update fail\n");
    // }

    // return 0;
}
