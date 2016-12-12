# ************************************************************
# A library for poker and majang application.
# It's provide common poker and majang algorithm.
#
# Copyright (C) 2013 Huang Tao <huangtao117@gmail.com>
# ************************************************************

# Set projectname 
project(libqipai)

# CMake policies
cmake_minimum_required(VERSION 2.8)

if(LINUX32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
elseif(LINUX64)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
endif(LINUX32)

# Add core sources
add_subdirectory(src)

# Add test sources
add_subdirectory(test)
