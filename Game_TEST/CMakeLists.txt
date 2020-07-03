PROJECT(GameTEST)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_EXECUTABLE(GAME_TEST
        src/entry_point.cpp src/SystemImpl.cpp src/SystemImpl.hpp src/ResourceStreamSystemImpl.cpp src/ResourceStreamSystemImpl.hpp)

IF (UNIX)
    TARGET_SOURCES(GAME_TEST PRIVATE src/PosixInput.cpp src/PosixInput.hpp)
    TARGET_COMPILE_DEFINITIONS(GAME_TEST PUBLIC HAS_POSIX_INPUT)
ENDIF ()

target_link_libraries(GAME_TEST PUBLIC Engine01 project_options project_warnings)
