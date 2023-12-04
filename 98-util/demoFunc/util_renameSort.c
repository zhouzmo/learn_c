#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILENAME_LENGTH 256

void renameAndSortFiles(const char *dirPath, const char *targetFile, const char *newFile) {
    DIR *dir;
    struct dirent *entry;
    char filenames[MAX_FILENAME_LENGTH][MAX_FILENAME_LENGTH];
    int numFiles = 0;

    // Open the directory
    dir = opendir(dirPath);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path of the entry
        char filePath[MAX_FILENAME_LENGTH];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        // Check if the entry is a directory
        if (entry->d_type == DT_DIR) {
            // Recursively process subdirectories
            renameAndSortFiles(filePath, targetFile, newFile);
        } else if (entry->d_type == DT_REG) {
            // Check if the entry matches the target file
            if (strcmp(entry->d_name, targetFile) == 0) {
                // Rename the file
                char newFilePath[MAX_FILENAME_LENGTH];
                snprintf(newFilePath, sizeof(newFilePath), "%s/%s", dirPath, newFile);
                if (rename(filePath, newFilePath) != 0) {
                    perror("Error renaming file");
                    continue;
                }
            }

            // Store the filename for sorting
            strcpy(filenames[numFiles], entry->d_name);
            numFiles++;
        }
    }

    // Sort the filenames in ascending order
    for (int i = 0; i < numFiles - 1; i++) {
        for (int j = 0; j < numFiles - i - 1; j++) {
            if (strcmp(filenames[j], filenames[j + 1]) > 0) {
                char temp[MAX_FILENAME_LENGTH];
                strcpy(temp, filenames[j]);
                strcpy(filenames[j], filenames[j + 1]);
                strcpy(filenames[j + 1], temp);
            }
        }
    }

    // Print the sorted filenames
    for (int i = 0; i < numFiles; i++) {
        printf("%s\n", filenames[i]);
    }

    // Close the directory
    closedir(dir);
}

int main() {
    const char *dirPath = "."; // 指定目录路径
    const char *targetFile = "07"; // 需要修改的文件名
    const char *newFile = "08"; // 修改后的文件名

    renameAndSortFiles(dirPath, targetFile, newFile);

    return 0;
}