PROJECT(Engine01_Utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_Utils STATIC
        include/Engine/string_view_utils.hpp
        src/string_view_utils.cpp
        )

target_include_directories(Engine01_Utils PRIVATE src)

target_include_directories(Engine01_Utils PUBLIC include)
target_link_libraries(Engine01_Utils PUBLIC magic_enum project_options project_warnings)
