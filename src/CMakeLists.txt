#
# A library for poker and majang application.
# It's provide common poker and majang algorithm.
#
# Copyright (C) 2013 Huang Tao <huangtao117@gmail.com>
#

# Set projectname
project(qipai)

# CMake policies
cmake_minimum_required(VERSION 2.8)

# define all source files
set(sources
    qp_common.c
    mj.c
    mj_algo.c
    mjhz.c
    card.c
    card_algo.c
    gp.c
)

# defile all header files
set(headers
    qipai.h
    qp_common.h
    mj.h
    mj_algo.h
    mjhz.h
    card.h
    card_algo.h
    gp.h
)

# add all .h files to a group(visual studio)
source_group("Header Files" FILES ${headers})

#libqipai.a for release
#libqipai-d.a for debug
#set(CMAKE_DEBUG_POSTFIX -d)

# 生成静态库
add_library(qipai STATIC ${sources} ${headers})
#add_library(libqipai SHARED ${sources} ${headers})
#add_library(libqipai-s STATIC ${sources} ${headers})
#set_target_properties(libqipai-s PROPERTIES OUTPUT_NAME "libqipai" PREFIX "lib")

# 链接库
target_link_libraries(qipai m)

# 设置输出目录
if(ANDROID)
message(STATUS "android doesn't need ARCHIVE_OUTPUT_DIRECTORY")
else(ANDROID)
set_target_properties(qipai
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
endif(ANDROID)

# add the install targets
install(TARGETS qipai DESTINATION lib)
install(FILES ${headers} DESTINATION include)
