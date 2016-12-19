@echo off
mkdir build.win
cd build.win
::cmake -G"NMake Makefiles" ..
::nmake

:: 32位
cmake -G"Visual Studio 14 2015" ..
:: 64位
::cmake -G"Visual Studio 14 2015 Win64"



