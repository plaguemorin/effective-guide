PROJECT(Engine01_Configuration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_Configuration STATIC
        include/Engine/Configuration/Entry.hpp
        include/Engine/Configuration/ParserListener.hpp

        src/INIReader.hpp
        src/StreamIniReader.cpp
        include/Engine/Configuration/Parser.hpp)

target_include_directories(Engine01_Configuration PRIVATE src)

target_include_directories(Engine01_Configuration PUBLIC include)
target_link_libraries(Engine01_Configuration PUBLIC project_options project_warnings Engine01_Logger Engine01_Stream)
