#!/bin/bash

BUILD_DIR=linux-x64
cmake -S .. -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug
cmake --build $BUILD_DIR -j
