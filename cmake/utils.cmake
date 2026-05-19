
#[==[
# @function addCopyFinallyBinaryFileTarget(<finalBinaryFileProjectName> <finalBinaryFileName> <finalBinaryFileDir>)
#
# 用于新增一个构建目标，这个目标用于将生成的二进制文件拷贝到 bin 目录以便于调试工具指定路径
# @param finalBinaryFileProjectName 生成这个二进制文件的目标名称
# @param finalBinaryFileName 这个二进制文件的名称
# @param finalBinaryFileDir 这个二进制文件所在的路径
]==]
function(addCopyFinallyBinaryFileTarget finalBinaryFileProjectName finalBinaryFileName finalBinaryFileDir)
    add_custom_command(
        TARGET ${finalBinaryFileProjectName} POST_BUILD
        COMMENT "Copy the binary file from cmake process binary dir to project_root/bin dir."
        COMMAND ${CMAKE_COMMAND} -E
        copy "${finalBinaryFileDir}/${finalBinaryFileName}"
        "${CMAKE_SOURCE_DIR}/bin/${finalBinaryFileName}"
        VERBATIM
    )

    set_property(DIRECTORY PROPERTY ADDITIONAL_CLEAN_FILES
        "${CMAKE_SOURCE_DIR}/bin"
    )

    message(STATUS "[${PROJECT_NAME}](function `${CMAKE_CURRENT_FUNCTION}\'): \n"
        "        finalBinaryFileProjectName: ${finalBinaryFileProjectName}\n"
        "        finalBinaryFileName: ${finalBinaryFileName}\n"
        "        finalBinaryFileDir: ${finalBinaryFileDir}"
    )
endfunction()
