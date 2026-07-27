"""
emdevif_stm32cubemx_processor 是一个用于将 STM32CubeMX 生成的 CMakeLists.txt 进行处理以适配 emdevif
的一个脚本

Requires:
    Python3

Example:
    python stm32cubemx_processor.py <process file>       -- if process file is relative path
    python stm32cubemx_processor.py <process file> -a    -- if process file is absolute path
"""

import sys
import os


def get_caller_path():
    """
    获得调用这个程序的绝对路径
    :return: 调用这个程序的绝对路径
    """
    return os.path.abspath(os.getcwd())


def add_comment_for_not_use_command(src: list[str], _i: int, comment: str, command: str) -> int:
    """
    根据特征注释与语句注释掉相应语句
    :param src: 被逐行拆分成字符串的文件
    :param _i: 当前待判断的行数
    :param comment: 要注释掉的语句上一行的注释
    :param command: 要替注释掉的语句
    :return: 处理后的文件行数
    """
    if src[_i].find(comment) != -1:
        _i += 1
        if src[_i].find(command) != -1:
            if src[_i][0] != "#":
                _result_str = "#" + src[_i]
                src[_i] = _result_str

    return _i


if __name__ == "__main__":
    argc = len(sys.argv)
    if argc != 2 and argc != 3:
        print("Use: python stm32cubemx_processor.py <process file>      -- if process file is relative path", "\n",
              "Or: python stm32cubemx_processor.py <process file> -a    -- if process file is absolute path")
        exit(1)

    argv_file_name = sys.argv[1]
    argv_file_name.replace("\\", "/")

    if argc == 3 and sys.argv[2] == "-a":
        # 本身就是绝对路径
        process_file_name = argv_file_name
    else:
        # 传入的相对路径，则转化为绝对路径
        process_file_name = get_caller_path()
        process_file_name.replace("\\", "/")
        process_file_name += "/" + argv_file_name

    process_file = open(process_file_name, "r")

    file_str = process_file.readlines()

    process_file.close()

    WILL_REPLACE_COMMENT_FLAG1: str = "# Link directories setup"
    WILL_REPLACE_COMMAND1: str = "target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})"
    WILL_REPLACE_COMMENT_FLAG2: str = "# Add libraries to the project"
    WILL_REPLACE_COMMAND2: str = "target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})"

    i: int = 0
    while i < len(file_str):
        i = add_comment_for_not_use_command(file_str, i, WILL_REPLACE_COMMENT_FLAG1, WILL_REPLACE_COMMAND1)
        i = add_comment_for_not_use_command(file_str, i, WILL_REPLACE_COMMENT_FLAG2, WILL_REPLACE_COMMAND2)

        i += 1

    result_str = ""
    for i in range(0, len(file_str)):
        result_str += file_str[i]

    process_file = open(process_file_name, "w")
    process_file.write(result_str)
    process_file.close()

    exit(0)
