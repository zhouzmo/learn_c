#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


/* 返回值: n表示读到了一行数据的数据个数(n >= 0)
 *         -1(读到文件尾部或者出错)
 */
static int read_line(int fd, unsigned char *buf)
{
    /* 循环读入一个字符 */
    /* 如何判断已经读完一行? 读到0x0d, 0x0a */

    unsigned char c;
    int len;
    int i = 0;
    int err = 0;

    while (1)
    {
        len = read(fd, &c, 1);
        if (len <= 0)
        {
            err = -1;
            break;
        }
        else
        {
            if (c != '\n' && c != '\r')
            {
                buf[i] = c;
                i++;
            }
            else
            {
                /* 碰到回车换行   */
                err = 0;
                break;
            }
        }
    }

    buf[i] = '\0';

    if (err && (i == 0))
    {
        /* 读到文件尾部了并且一个数据都没有读进来 */
        return -1;
    }
    else
    {
        return i;
    }
}


void process_data(unsigned char *data_buf, unsigned char *result_buf)
{
    /* 示例1: data_buf = ",语文,数学,英语,总分,评价" 
     *        result_buf = ",语文,数学,英语,总分,评价" 
     * 示例2: data_buf = "张三,90,91,92,," 
     *        result_buf = "张三,90,91,92,273,A+" 
     *
     */

    char name[100];
    int scores[3];
    int sum;
    char *levels[] = {"A+", "A", "B"};
    int level;

    if (data_buf[0] == 0xef) /* 对于UTF-8编码的文件,它的前3个字符是0xef 0xbb 0xbf */
    {
        strcpy(result_buf, data_buf);
    }
    else
    {
        sscanf(data_buf, "%[^,],%d,%d,%d,", name, &scores[0], &scores[1], &scores[2]);
        //printf("result: %s,%d,%d,%d\n\r", name, scores[0], scores[1], scores[2]);
        //printf("result: %s --->get name---> %s\n\r", data_buf, name);
        sum = scores[0] + scores[1] + scores[2];
        if (sum >= 270)
            level = 0;
        else if (sum >= 240)
            level = 1;
        else
            level = 2;

        sprintf(result_buf, "%s,%d,%d,%d,%d,%s", name, scores[0], scores[1], scores[2], sum, levels[level]);
        //printf("result: %s", result_buf);
    }
}



int main(int argc, char **argv)
{
    int fd_data, fd_result;
    int i;
    int len;
    unsigned char data_buf[1000];
    unsigned char result_buf[1000];
    
    if (argc != 3)
    {
        printf("Usage: %s <data csv file> <result csv file>\n", argv[0]);
        return -1;
    }

    fd_data = open(argv[1], O_RDONLY);
    // if (fd_data < 0)
    // {
    //     printf("can not open file %s\n", argv[1]);
    //     perror("open");
    //     return -1;
    // }
    // else
    // {
        printf("data file fd = %d\n", fd_data);
    // }

    fd_result = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    // if (fd_result < 0)
    // {
    //     printf("can not create file %s\n", argv[2]);
    //     perror("create");
    //     return -1;
    // }
    // else
    // {
        printf("resultfile fd = %d\n", fd_result);
    // }


    while (1)
    {
        /* 从数据文件里读取1行 */
        len = read_line(fd_data, data_buf);
        if (len == -1)
        {
            break;
        }

        //if (len != 0)
        //    printf("line: %s\n\r", data_buf);

        if (len != 0)
        {
            /* 处理数据 */
            process_data(data_buf, result_buf);
            
            /* 写入结果文件 */
            //write_data(fd_result, result_buf);
            write(fd_result, result_buf, strlen(result_buf));

            write(fd_result, "\r\n", 2);
        }

    }

    close(fd_data);
    close(fd_result);

    return 0;
}


/*
$ gcc process_excel.c
$ ./a.out score.csv result.csv
data file fd = 3
resultfile fd = 4
$ cat result.csv
,语文,数学,英语,总分,评价
张三,90,91,92,273,A+
李四,80,81,82,243,A
王五,70,71,72,213,B




# 0d 0a 是换行符
$ hexdump -C score.csv
00000000  ef bb bf 2c e8 af ad e6  96 87 2c e6 95 b0 e5 ad  |...,......,.....|
00000010  a6 2c e8 8b b1 e8 af ad  2c e6 80 bb e5 88 86 2c  |.,......,......,|
00000020  e8 af 84 e4 bb b7 0d 0a  e5 bc a0 e4 b8 89 2c 39  |..............,9|
00000030  30 2c 39 31 2c 39 32 2c  2c 0d 0a e6 9d 8e e5 9b  |0,91,92,,.......|
00000040  9b 2c 38 30 2c 38 31 2c  38 32 2c 2c 0d 0a e7 8e  |.,80,81,82,,....|
00000050  8b e4 ba 94 2c 37 30 2c  37 31 2c 37 32 2c 2c 0d  |....,70,71,72,,.|
00000060  0a                                                |.|
00000061





*/