#!/bin/sh
# 为了同时构建32位和64位
# 我们需要安装必要的包
# 32位时指定CXX_FLAGS = m32
sudo apt-get install libc6-dev-i386
sudo apt-get install lib32stdc++6

LIBNAME="qipai"

rm -rf prebuilt/linux

mkdir build.linux
cd build.linux
cmake -DCMAKE_BUILD_TYPE=Release -DLINUX32=1 ..

make

cd ..
mkdir -p prebuilt/linux/32-bit
cp build.linux/lib/lib"${LIBNAME}".a prebuilt/linux/32-bit/
rm -rf build.linux

mkdir build.linux
cd build.linux
cmake -DCMAKE_BUILD_TYPE=Release -DLINUX64=1 ..

make

cd ..
mkdir -p prebuilt/linux/64-bit
cp build.linux/lib/lib"${LIBNAME}".a prebuilt/linux/64-bit/
rm -rf build.linux

