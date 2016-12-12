#!/bin/sh

sudo apt-get install libc6-dev-i386

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

