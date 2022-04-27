# CMakeList.txt: 5173io-dll-injector 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# 将源代码添加到此项目的可执行文件。
add_executable (5173io-dll-injector "main.cpp" "5173io-dll-injector.cpp" "5173io-dll-injector.h")

# TODO: 如有需要，请添加测试并安装目标。
