# ************************************************************
# A library for poker and majang application.
# It's provide common poker and majang algorithm.
#
# Copyright (C) 2013 Huang Tao <huangtao117@gmail.com>
# ************************************************************

# CMake版本要求
cmake_minimum_required(VERSION 2.8)

# 项目名称 
project(qipai)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

if(LINUX32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
elseif(LINUX64)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
endif(LINUX32)

# 包含子目录
add_subdirectory(src)
add_subdirectory(test)
