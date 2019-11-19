PROJECT(bmp_png)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Bitmap_PNG
        STATIC
        include/Loaders/BitmapLoader.hpp
        src/LODEPNG.H
        src/LODEPNG.CPP
        src/gif_load.hpp
        src/BitmapLoader.cpp)

target_include_directories(Bitmap_PNG PUBLIC include)
target_link_libraries(Bitmap_PNG PUBLIC Engine01)
target_compile_options(Bitmap_PNG PUBLIC -fno-rtti)
target_compile_definitions(Bitmap_PNG PRIVATE -DLODEPNG_NO_COMPILE_DISK -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_CPP)
