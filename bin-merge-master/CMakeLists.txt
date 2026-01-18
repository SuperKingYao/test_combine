cmake_minimum_required(VERSION 3.0.0) #指定CMake的最小版本
project(binMerge VERSION 0.1.0 DESCRIPTION 添加CRC校验工具 LANGUAGES C) #指定项目名称及版本号，初始化项目相关变量。
configure_file(${PROJECT_SOURCE_DIR}/Config.h.in ../config.h)
set(CMAKE_C_STANDARD 11) # C语言标准
set(CMAKE_C_EXTENSIONS ON) # 启用编译器拓展
set(CMAKE_C_STANDARD_REQUIRED ON) # 指定所选标准的版本
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG ") # 当构建debug工程时定义DEBUG宏
# add_compile_options(-Werror -Wall -Wextra -Wfatal-errors) #编译选项

#查找当前目录下的所有源文件
#并将名称保存到DIR_SRCS变量
aux_source_directory(. DIR_SRCS)




add_executable(${PROJECT_NAME} ${DIR_SRCS})


                           
