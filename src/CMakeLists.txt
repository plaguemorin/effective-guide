PROJECT(DOS_GAME01)
cmake_minimum_required(VERSION 3.9)

include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(
        GAME01
        DosSystem.hpp DosSystem.cpp
        DosMode13.hpp DosMode13.cpp
        main.cpp
)
target_include_directories(GAME01 PRIVATE src)
target_link_libraries(GAME01 PUBLIC Engine01 Bitmap_PNG)
target_compile_options(GAME01 PUBLIC -fno-rtti)

check_ipo_supported(RESULT result)
if (result)
    message(STATUS "LTO Enabled")
    set_property(TARGET GAME01 PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif ()
