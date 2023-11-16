#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include <linux/input.h>

#include <sys/ioctl.h>

#include <tslib.h>

/*
实现一个程序，不断打印2个触点的距离。
思路：假设是5点触摸屏，调用一次ts_read_mt可以得到5个新数据；
使用新旧数据进行判断，如果有2个触点，就打印出距离。



*/

int distance(struct ts_sample_mt *point1, struct ts_sample_mt *point2)
{
	int x = point1->x - point2->x;
	int y = point1->y - point2->y;
	// 需要开平方根
	return x * x + y * y;
}

int main(int argc, char **argv)
{
	struct tsdev *ts;
	int i;
	int ret;
	struct ts_sample_mt **samp_mt;
	struct ts_sample_mt **pre_samp_mt;
	int max_slots;
	int point_pressed[20];
	struct input_absinfo slot;
	int touch_cnt = 0;

	/*

	step 1:
	ts_open 通过 环境变量，默认变量，扫描设备节点，找到 ts 设备
	step 2:
	ts_config 读取配置文件 moudle，构建 tsdev的 list 结构体

	在 read 时，以递归的方式，将数据按照 list 中不同的方式读取

	*/
	ts = ts_setup(NULL, 0);
	if (!ts)
	{
		perror("ts_setup");
		return errno;
	}
	// 获取最大的触点数量
	if (ioctl(ts_fd(ts), EVIOCGABS(ABS_MT_SLOT), &slot) < 0)
	{
		perror("ioctl EVIOGABS");
		ts_close(ts);
		return errno;
	}

	max_slots = slot.maximum + 1 - slot.minimum;

	samp_mt = malloc(sizeof(struct ts_sample_mt *));
	if (!samp_mt)
	{
		ts_close(ts);
		return -ENOMEM;
	}
	samp_mt[0] = calloc(max_slots, sizeof(struct ts_sample_mt));
	if (!samp_mt[0])
	{
		free(samp_mt);
		ts_close(ts);
		return -ENOMEM;
	}

	pre_samp_mt = malloc(sizeof(struct ts_sample_mt *));
	if (!pre_samp_mt)
	{
		ts_close(ts);
		return -ENOMEM;
	}
	pre_samp_mt[0] = calloc(max_slots, sizeof(struct ts_sample_mt));
	if (!pre_samp_mt[0])
	{
		free(pre_samp_mt);
		ts_close(ts);
		return -ENOMEM;
	}

	// 将 valid = 0，表示数据无变化，即垃圾数据
	for (i = 0; i < max_slots; i++)
	{
		pre_samp_mt[0][i].valid = 0;
	}

	while (1)
	{
		// ts ts设备，samp_mt事件信息结构体，max_slots 最大触点数量，读 1 次
		ret = ts_read_mt(ts, samp_mt, max_slots, 1);

		if (ret < 0)
		{
			perror("ts_read_mt");
			free(samp_mt);
			ts_close(ts);
			exit(1);
		}

		// valid != 0 ,表示数据更新了,保存数据
		for (i = 0; i < max_slots; i++)
		{

			if (samp_mt[0][i].valid)
			{
				memcpy(&pre_samp_mt[0][i], &samp_mt[0][i], sizeof(struct ts_sample_mt));
			}
		}

		touch_cnt = 0;
		// 判断有效数据中是否存在两个数据
		for (i = 0; i < max_slots; i++)
		{
			// 数据有效，且触点未被松开
			if (pre_samp_mt[0][i].valid && pre_samp_mt[0][i].tracking_id != -1)
			{
				point_pressed[touch_cnt++] = i;
			}
		}

		if (touch_cnt == 2)
		{ // 计算触点距离
			printf("distance: %08d\n",
				   distance(&pre_samp_mt[0][point_pressed[0]], &pre_samp_mt[0][point_pressed[1]]));
		}
	}

	return 0;
}

/*
$ arm-buildroot-linux-gnueabihf-gcc mt_cal_distance-zmo.c -o mt_cal_distance-zmo.out -lts

./mt_cal_distance-zmo.out
distance: 00579805
distance: 00162461
distance: 00080648
distance: 00014522



*/