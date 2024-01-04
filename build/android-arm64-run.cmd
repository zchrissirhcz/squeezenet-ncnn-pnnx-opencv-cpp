@echo off

set BUILD_DIR=android-arm64
set DST_DIR=/data/local/tmp
set EXE_FILE=testbed

adb shell "mkdir -p %DST_DIR%"
adb push %BUILD_DIR%/%EXE_FILE% %DST_DIR%
@REM adb push ../bg.jpg %DST_DIR%
adb shell "cd %DST_DIR%; chmod +x %DST_DIR%/%EXE_FILE%; ./%EXE_FILE%"