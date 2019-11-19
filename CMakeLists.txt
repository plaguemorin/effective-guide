PROJECT(GAME01)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Engine)
add_subdirectory(bmp_png)

add_executable(
        GAME01
        src/DosSystem.hpp src/DosSystem.cpp
        src/DosMode13.hpp src/DosMode13.cpp
        src/main.cpp
)

target_include_directories(GAME01 PUBLIC src)
target_link_libraries(GAME01 PUBLIC Engine01 Bitmap_PNG)
target_compile_options(GAME01 PUBLIC -fno-rtti)

add_executable(TEST_MAP test/test_map.cpp)
target_link_libraries(TEST_MAP PUBLIC Engine01)
