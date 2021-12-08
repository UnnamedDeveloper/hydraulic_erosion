@echo off

mkdir bin
cd bin

if "%1" == "release" goto buildRelease
if "%1" == "debug" goto buildDebug

goto buildIde

:buildRelease
mkdir release
cd release
cmake -G "MinGW Makefiles" ..\.. -DCMAKE_BUILD_TYPE=Release
mingw32-make
goto exit

:buildDebug
mkdir debug
cd debug
cmake -G "MinGW Makefiles" ..\.. -DCMAKE_BUILD_TYPE=Debug
mingw32-make
goto exit

:buildIde
mkdir vs
cd vs
cmake ..\..
goto exit

:exit
pause
