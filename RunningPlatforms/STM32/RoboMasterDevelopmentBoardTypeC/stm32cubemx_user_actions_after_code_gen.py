#!/usr/bin/env python

# 由于 STM32CubeMX 的 User Actions 功能有限，无法直接支持复杂的操作（只能执行可执行文件/脚本，不能执行命令），
# 因此使用这个脚本调用 stm32cubemx_processor.py 来处理生成的 CMakeLists.txt 文件。

import os


def get_caller_path():
    """
    获得调用这个程序的绝对路径
    :return: 调用这个程序的绝对路径
    """
    return os.path.abspath(os.getcwd())


if __name__ == "__main__":
    cwd: str = get_caller_path()
    cwd.replace("\\", "/")
    print(" * cwd is: ", cwd)

    processor_file_relative_path = "emdevif_collection/emdevif/support/stm32cubemx/stm32cubemx_processor.py"

    cmd: str = f"python \"{cwd}/../../../{processor_file_relative_path}\" \"{cwd}/cmake/stm32cubemx/CMakeLists.txt\" -a"
    cmd.replace("\\", "/")
    print(f" * Running command: {cmd}")

    if os.system(cmd) != 0:
        print("Error: stm32cubemx_processor.py process failed!")
        exit(1)
