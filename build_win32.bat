@echo off

mkdir bin
cd bin

if "%1" == "release" goto buildRelease
if "%1" == "debug" goto buildDebug

echo "Missing parameter 1"
echo "Usage: build_win32.bat [debug/release]"
goto exit

:buildRelease
mkdir release
cd release
cmake -G "MinGW Makefiles" ..\.. -DCMAKE_BUILD_TYPE=Release
mingw32-make
goto exit

:buildDebug
mkdir debug
cd debug
cmake ..\.. -DCMAKE_BUILD_TYPE=Debug
goto exit

:exit
pause
