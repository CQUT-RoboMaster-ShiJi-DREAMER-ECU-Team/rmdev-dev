#[==[
# @file utility.cmake
# @brief CMake 实用功能
]==]

#[==[
# @function emdevif_trueOptionsCount(<out_result> ...)
#
# 用于获取给定的布尔类型变量中，真值的数量
# @param[out] out_result 返回值为真的变量数
# @param ... 待计数的变量。可以有多个，用空格分隔。
]==]
function(emdevif_trueOptionsCount out_result)
    set(trueCount 0)

    foreach (item IN LISTS ARGN)
        if (${item})
            math(EXPR trueCount "${trueCount} + 1")
        endif ()
    endforeach ()

    set(${out_result} ${trueCount} PARENT_SCOPE)
endfunction()

#[==[
# @function emdevif_assertMutuallyExclusiveOptions(...)
#
# 用于断言给定的布尔类型变量中，真值的数量最多 1 个
# @param ... 待断言的变量。可以有多个，用空格分隔。
]==]
function(emdevif_assertMutuallyExclusiveOptions)
    emdevif_trueOptionsCount(result ${ARGN})

    if (result GREATER_EQUAL 2)
        message(FATAL_ERROR "Assert `mutuallyExclusiveOptions\' Failed.")
    endif ()
endfunction()

#[==[
# @function emdevif_listToCCppStringTypeInitializerList(<out_var> <inputList>)
#
# 将 CMake 的分号分隔列表转化为 C/C++ 的字符串类型的初始化列表。
# 例如："foo;bar;c" 将被转化为 {"foo", "bar", "c"} ，可以用于 C/C++
# 字符串数组等能够使用 initializer_list<const char*> 形式初始化的类型。
# @note 会对输入的列表进行以下额外的操作：去除列表内每个元素的前缀、后缀空格；
# 去除重复的元素，仅保留第一个。
# @param[out] out_var 转化后的初始化列表
# @param inputList 待转化的列表类型
]==]
function(emdevif_listToCCppStringTypeInitializerList out_var inputList)
    if ("${inputList}" STREQUAL "")
        set(${out_var} [[{""}]] PARENT_SCOPE)
        return ()
    endif ()

    set(inputList_ ${inputList})

    list(TRANSFORM inputList_ STRIP)
    list(REMOVE_DUPLICATES inputList_)

    list(TRANSFORM inputList_ PREPEND [["]])
    list(TRANSFORM inputList_ APPEND [["]])

    string(REPLACE ";" ", " outputInitializerList "${inputList_}")
    string(PREPEND outputInitializerList "{")
    string(APPEND outputInitializerList "}")

    set(${out_var} ${outputInitializerList} PARENT_SCOPE)
endfunction()
