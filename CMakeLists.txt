#
# A library for poker and majang application.
# It's provide common poker and majang algorithm.
#
# Copyright (C) 2013 Huang Tao <huangtao117@gmail.com>
#

# Set projectname 
project(libqp)

# CMake policies
cmake_minimum_required(VERSION 2.8)

# Add core sources
add_subdirectory(src)

# Add test sources
add_subdirectory(test)
