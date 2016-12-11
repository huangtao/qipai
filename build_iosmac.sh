#!/bin/sh


LIBNAME="qipai"
IOS_SDK_VERSION="10.1"

rm -rf prebuilt/ios
rm -rf prebuilt/mac

# 使用ios-cmake产生ios项目
rm -rf build.ios
mkdir build.ios
cd build.ios

cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/iOS_64.cmake -DCMAKE_IOS_DEVELOPER_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/ -GXcode ..



