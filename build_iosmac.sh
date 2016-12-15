#!/bin/sh

LIBNAME="qipai"
IOS_SDK_VERSION="10.1"

rm -rf prebuilt/ios
rm -rf prebuilt/mac

# 使用ios-cmake产生ios项目
rm -rf build.ios
mkdir build.ios
cd build.ios

cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/iOS.cmake -DCMAKE_IOS_DEVELOPER_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/ -GXcode ..

# 构建iphone模拟器64bit
#xcodebuild -project qipai.xcodeproj -alltargets -sdk iphonesimulator"${IOS_SDK_VERSION}" -configuration Release
#xcodebuild -project qipai.xcodeproj -alltargets -sdk iphonesimulator -arch i386 -arch x86_64 -configuration Release
xcodebuild -project qipai.xcodeproj -alltargets -sdk iphonesimulator -arch x86_64 -configuration Release

cd ..
mkdir -p lib/x86_64
cp build.ios/lib/Release/lib"${LIBNAME}".a lib/x86_64

# 构建iphone os
#xcodebuild -project build.ios/qipai.xcodeproj -alltargets -sdk iphoneos"${IOS_SDK_VERSION}" -configuration Release
#xcodebuild -project build.ios/qipai.xcodeproj -alltargets -sdk iphoneos -arch armv7 -arch armv7s -configuration Release
xcodebuild -project build.ios/qipai.xcodeproj -alltargets -sdk iphoneos -arch arm64 -configuration Release

mkdir -p lib/arm64
cp build.ios/lib/Release/lib"${LIBNAME}".a lib/arm64

# 生产胖包
lipo lib/x86_64/lib"${LIBNAME}".a lib/arm64/lib"${LIBNAME}".a -create -output lib"${LIBNAME}".a
lipo -info lib"${LIBNAME}".a

mkdir -p prebuilt/ios
mv lib"${LIBNAME}".a prebuilt/ios
#rm -rf lib
#rm -rf build.ios

echo
echo "=============================="
echo "finished build ios fat library"
echo "=============================="
echo

# Mac版本
rm -rf build.mac
mkdir build.mac
cd build.mac
cmake -G Xcode ..

xcodebuild -project qipai.xcodeproj -alltargets -arch x86_64 -configuration Release

cd ..
mkdir -p prebuilt/mac
cp build.mac/lib/Release/lib"${LIBNAME}".a prebuilt/mac/lib"${LIBNAME}".a
#rm -rf build.mac

echo
echo "=============================="
echo "finished build mac library"
echo "=============================="
echo
