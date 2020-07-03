cmake_minimum_required(VERSION 3.15)

project(GAME01)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)

# Common targets
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which
    # is a good idea if any #define's change
    #
    #
    target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)
endif ()

add_subdirectory(vendor/magic_enum)
add_subdirectory(lodepng)
add_subdirectory(Engine)
add_subdirectory(Game_TEST)
add_subdirectory(Game_WIN)

    add_subdirectory(Game_DOS)
