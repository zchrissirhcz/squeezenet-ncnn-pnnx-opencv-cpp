#!/bin/bash

if [ x"${ANDROID_NDK}" == "x" ]; then 
    ANDROID_NDK=~/soft/android-ndk-r21e
fi

TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake

echo "ANDROID_NDK is $ANDROID_NDK"
echo "TOOLCHAIN is: $TOOLCHAIN"

BUILD_DIR=android-arm64
cmake \
    -S .. \
    -B $BUILD_DIR \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -DANDROID_ABI="arm64-v8a" \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release

#ninja
#cmake --build $BUILD_DIR --verbose
cmake --build $BUILD_DIR -j4
