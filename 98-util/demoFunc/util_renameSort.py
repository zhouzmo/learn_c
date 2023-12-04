import os

def rename_files(dir_path):
    # 打开路径文件，没有就是当前路径
    if dir_path == "":
        dir_path = os.getcwd()

    # 将目录下的子目录、文件都放入列表中
    entries = os.listdir(dir_path)
    entries.sort()

    # 存储文件前缀和对应的子目录名
    prefix_map = {}

    for entry in entries:
        entry_path = os.path.join(dir_path, entry)

        if os.path.isdir(entry_path):
            # 判断是子目录，修改路径名
            new_entry = entry
            prefix = entry.split('-')[0]  # 获取文件前缀
            prefix_map[prefix] = entry
        else:
            # 判断是文件，修改文件名
            prefix = entry.split('-')[0]  # 获取文件前缀
            if prefix in prefix_map:
                new_entry = prefix + "-" + entry
                prefix_map[prefix] = entry
            else:
                new_entry = entry

        # 修改文件名或路径名
        new_entry_path = os.path.join(dir_path, new_entry)
        os.rename(entry_path, new_entry_path)

    # 依次取出前缀，修改文件名
    for prefix in prefix_map:
        entry = prefix_map[prefix]
        new_entry = entry.replace(prefix + "-", str(prefix) + "-")
        entry_path = os.path.join(dir_path, entry)
        new_entry_path = os.path.join(dir_path, new_entry)
        os.rename(entry_path, new_entry_path)

# 指定目录路径
dir_path = ""

# 调用函数
rename_files(dir_path)