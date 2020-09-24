PROJECT(GameTEST)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (MSVC)
    SET(OPTIONS WIN32)
ENDIF (MSVC)


add_executable(Game_WIN WIN32
        src/main.cpp
        src/Win32System.cpp
        src/Win32System.hpp
        src/Win32Window.hpp
        src/Win32Window.cpp
        src/Win32FileStream.cpp
        src/Win32FileStream.hpp src/Win32StreamFactory.cpp src/Win32StreamFactory.hpp src/string_2_wstring.hpp src/RenderWindow.cpp src/RenderWindow.hpp)

target_include_directories(Game_WIN PUBLIC
        $ENV{UniversalCRTSdkDir}/Include/ucrt
        $ENV{UniversalCRTSdkDir}/Include/10.0.19041.0/ucrt
        )

target_link_libraries(Game_WIN PUBLIC Engine01 project_options project_warnings)
target_compile_definitions(Game_WIN PUBLIC WIN32_LEAN_AND_MEAN UNICODE NOMINMAX)

set_target_properties(Game_WIN 
    PROPERTIES 
   VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../resources")
