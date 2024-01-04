#!/bin/bash

BUILD_DIR=mac-x64
cmake -S .. -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug
cmake --build $BUILD_DIR -j
