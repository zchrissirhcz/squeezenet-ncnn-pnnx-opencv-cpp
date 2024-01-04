@echo off

if not defined ANDROID_NDK (
  set ANDROID_NDK=E:/soft/Android/ndk-r21e
)
set TOOLCHAIN=%ANDROID_NDK%/build/cmake/android.toolchain.cmake

echo "ANDROID_NDK is %ANDROID_NDK%"
echo "TOOLCHAIN is: %TOOLCHAIN%"
echo "ARTIFACTS_DIR is %ARTIFACTS_DIR%"

set BUILD_DIR=android-arm64
cmake ^
  -S .. ^
  -B %BUILD_DIR% ^
  -G Ninja ^
  -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN% ^
  -DANDROID_ABI="arm64-v8a" ^
  -DANDROID_PLATFORM=android-24 ^
  -DCMAKE_BUILD_TYPE=Release

cmake --build %BUILD_DIR%
