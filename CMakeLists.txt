PROJECT(GAME01)
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Engine)
add_subdirectory(bmp_png)
if (MAKE_DOS)
    add_subdirectory(src)
endif ()

add_executable(TEST_MAP test/test_map.cpp)
target_link_libraries(TEST_MAP PUBLIC Engine01)
