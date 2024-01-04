cmake_minimum_required(VERSION 3.25)
project(squeezenet_ncnn_pnnx_opencv_cpp)

set(CMAKE_CXX_STANDARD 11)

include(cmake/cvpkg.cmake)
include(cmake/output_dir2.cmake)

# https://github.com/Tencent/ncnn/releases
set(ncnn_DIR "D:/artifacts/ncnn/ncnn-20240102-windows-vs2022/x64/lib/cmake/ncnn")
find_package(ncnn REQUIRED)

# https://github.com/opencv/opencv/releases/
set(OpenCV_DIR "D:/artifacts/opencv/4.9.0/windows/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

add_executable(infer_squeezenet
    main.cpp
)
target_link_libraries(infer_squeezenet PRIVATE ncnn ${OpenCV_LIBS})

cvpkg_copy_required_dlls(infer_squeezenet ${CMAKE_BINARY_DIR})

add_custom_command(TARGET infer_squeezenet 
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/squeezenet_v1.1.param ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/squeezenet_v1.1.bin ${CMAKE_BINARY_DIR}/

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/squeezenet_v1.1.ncnn.param ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/squeezenet_v1.1.ncnn.bin ${CMAKE_BINARY_DIR}/

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/000017.jpg ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/cat.jpg ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/synset_words.txt ${CMAKE_BINARY_DIR}/
)
