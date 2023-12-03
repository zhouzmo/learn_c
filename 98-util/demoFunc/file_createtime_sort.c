#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define FOLDER_PATH "F:\\temp02" // 文件夹路径
#define START_NUM 1              // 开始序号
#define IS_CANCLE 0              // 0 命名 1撤销命名

typedef struct
{
    char name[256];
    int index;
    time_t modificationTime;
} FileInfo;

int compareFileInfo(const void *a, const void *b)
{
    FileInfo *fileInfoA = (FileInfo *)a;
    FileInfo *fileInfoB = (FileInfo *)b;
    return difftime(fileInfoA->modificationTime, fileInfoB->modificationTime);
}

int extractIndex(const char *fileName)
{
    int index = -1;
    sscanf(fileName, "%d-", &index);
    return index;
}

void renameFilesBack(const char *folderPath, FileInfo *fileInfoArray, int fileCount)
{
    for (int i = fileCount - 1; i >= 0; i--)
    {
        char oldFilePath[256];
        char newFilePath[256];

        // 移除文件名前 n 位
        char newFileName[256];
        strcpy(newFileName, fileInfoArray[i].name + 4);

        sprintf(oldFilePath, "%s/%s", folderPath, fileInfoArray[i].name);
        sprintf(newFilePath, "%s/%s", folderPath, newFileName);

        if (rename(oldFilePath, newFilePath) != 0)
        {
            printf("Failed to rename file: %s\n", fileInfoArray[i].name);
        }
        else
        {
            printf("Renamed file: %s\n", fileInfoArray[i].name);
        }
    }
}

int main(int argc, char *argv[])
{

    char *folderPath; // 替换为实际的文件夹路径
    int startNum;           // 开始序号
    int is_cancle;          // 0 命名 1撤销命名
    char currentPath[256];

    startNum = START_NUM;
    folderPath = FOLDER_PATH;
    is_cancle = IS_CANCLE;

    // 解析命令行参数
    if (argc >= 2)
    {
        startNum = atoi(argv[1]);
        if (getcwd(currentPath, sizeof(currentPath)) != NULL)
        {
            printf("Current Path: %s\n", currentPath);
            folderPath = currentPath;
        }
    }
    else
    {
        folderPath = FOLDER_PATH;
        startNum = START_NUM;
    }

    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    // 打开文件夹
    dir = opendir(folderPath);
    if (dir == NULL)
    {
        printf("Failed to open folder\n");
        return 1;
    }

    // 统计文件夹内视频文件数量
    int fileCount = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        char filePath[256];
        sprintf(filePath, "%s/%s", folderPath, entry->d_name);

        // 判断文件类型为普通文件并以.mp4, .avi, .mov结尾
        if (stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode) &&
            (strstr(entry->d_name, ".mp4") != NULL || strstr(entry->d_name, ".avi") != NULL || strstr(entry->d_name, ".mov") != NULL))
        {
            fileCount++;
        }
    }

    // 分配文件信息数组的内存
    FileInfo *fileInfoArray = (FileInfo *)malloc(fileCount * sizeof(FileInfo));

    // 重新遍历文件夹，将视频文件信息存入数组
    rewinddir(dir);
    int index = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        char filePath[256];
        sprintf(filePath, "%s/%s", folderPath, entry->d_name);

        // 判断文件类型为普通文件并以.mp4, .avi, .mov结尾
        if (stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode) &&
            (strstr(entry->d_name, ".mp4") != NULL ||
             strstr(entry->d_name, ".avi") != NULL ||
             strstr(entry->d_name, ".mov") != NULL))
        {
            FileInfo fileInfo;
            strcpy(fileInfo.name, entry->d_name);
            fileInfo.index = extractIndex(entry->d_name); // 提取文件名中的序号部分
            fileInfo.modificationTime = fileStat.st_mtime;
            fileInfoArray[index] = fileInfo;
            index++;
        }
    }

    // 关闭文件夹
    closedir(dir);

    // 按文件的修改时间进行排序
    qsort(fileInfoArray, fileCount, sizeof(FileInfo), compareFileInfo);

    // 修改文件名或撤销操作
    if (IS_CANCLE)
    {
        renameFilesBack(folderPath, fileInfoArray, fileCount);
    }
    else
    {
        for (int i = 0; i < fileCount; i++)
        {
            // 仅对序号不匹配的文件进行重命名
            if (fileInfoArray[i].index != startNum + i)
            {
                char oldFilePath[256];
                char newFilePath[256];
                sprintf(oldFilePath, "%s/%s", folderPath, fileInfoArray[i].name);
                sprintf(newFilePath, "%s/%03d-%s", folderPath, startNum + i, fileInfoArray[i].name);

                if (rename(oldFilePath, newFilePath) != 0)
                {
                    printf("Failed to rename file: %s\n", fileInfoArray[i].name);
                }
                else
                {
                    printf(": %03d-%s OK\n", startNum + i, fileInfoArray[i].name);
                }
            }
        }
    }

    // 释放内存
    free(fileInfoArray);

    return 0;
}
