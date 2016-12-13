@echo off
::
:: 32位
::
mkdir build.win
cd build.win
::cmake -G"NMake Makefiles" ..
::nmake

::cmake -G"Visual Studio 14 2015"

cmake -G"Visual Studio 14 2015 Win64"



