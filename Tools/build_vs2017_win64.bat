@echo off

REM cd /d %~dp0 checks out to the directory of the batch file
REM this is useful so we can always create the build dir in the right spot
setlocal
cd /d %~dp0

cd ..

IF NOT EXIST build mkdir build

cd build

cmake -G "Visual Studio 15 2017 Win64" ..
