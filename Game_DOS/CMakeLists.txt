PROJECT(DOS_GAME01)
cmake_minimum_required(VERSION 3.9)

include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        GAME01
        DosMode13.hpp DosMode13.cpp
        main.cpp
        StdFile.cpp
        StdFile.hpp
        DosDetect.cpp
        DosDetect.hpp
        KeyboardDecoding/KeyboardEventHandler.hpp
        KeyboardDecoding/KeyboardEventHandler.cpp
        KeyboardDecoding/KeyboardEventPauseHandler.hpp
        KeyboardDecoding/KeyboardEventHandlerResult.hpp
        KeyboardDecoding/KeyboardEventExtendedHandler.hpp
        KeyboardDecoding/KeyboardEventNormalHandler.hpp
        MouseInt33/MouseComDrv.cpp
        MouseInt33/MouseComDrv.hpp
        Signals.cpp
        Signals.hpp
        Timer.cpp
        Timer.hpp)
target_include_directories(GAME01 PRIVATE src)
target_link_libraries(GAME01 PUBLIC Engine01)
target_compile_options(GAME01 PUBLIC -fno-rtti)

check_ipo_supported(RESULT result)
if (result)
    message(STATUS "LTO Enabled")
    set_property(TARGET GAME01 PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif ()

configure_file(../resources/game.ini RES/GAME.INI NEWLINE_STYLE DOS)
