
#[==[
# addCopyFinallyBinaryFileTarget(<targetName> [executableSuffix])
#
# 新增一个用户自定义目标，用于将 targetName 目标生成的二进制文件拷贝到 bin 目录以便于调试工具指定路径
#
# targetName
#     待拷贝的文件对应的目标名称
#
# executableSuffix
#     可执行文件的后缀名（要包含 `.`）。此参数可选，如果不指定，则使用 CMAKE_EXECUTABLE_SUFFIX 变量的值作为后缀名。
]==]
function(addCopyFinallyBinaryFileTarget targetName)
    if (NOT ARGN)
        set(executableSuffix "${CMAKE_EXECUTABLE_SUFFIX}")
    else ()
        set(executableSuffix "${ARGN}")
    endif ()

    set(finalBinaryFileName "${targetName}${executableSuffix}")

    message(DEBUG "In function `${CMAKE_CURRENT_FUNCTION}`:")
    list(APPEND CMAKE_MESSAGE_INDENT "    ")

    message(DEBUG "executableSuffix: ${executableSuffix}")
    message(DEBUG "targetName: ${targetName}")
    message(DEBUG "finalBinaryFileName: ${finalBinaryFileName}")

    set(copyDestFile "${CMAKE_SOURCE_DIR}/bin/${finalBinaryFileName}")
    message(DEBUG "copyDestFile: ${copyDestFile}")

    add_custom_command(
        TARGET ${targetName} POST_BUILD
        COMMENT "Copy the binary file from cmake process binary dir to project_root/bin dir."
        COMMAND ${CMAKE_COMMAND} -E
        copy "$<TARGET_FILE:${targetName}>" "${copyDestFile}"
        VERBATIM
    )

    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        ${copyDestFile}
    )
endfunction()
