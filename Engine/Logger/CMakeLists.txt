PROJECT(Engine01_Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_Logger STATIC
        include/Logger/Logger.hpp
        include/Logger/LoggerSink.hpp
        include/Logger/SourceLocation.hpp
        include/Logger/Format.hpp
        src/Logger.cpp
        )

target_include_directories(Engine01_Logger PRIVATE src)

target_include_directories(Engine01_Logger PUBLIC include)
target_link_libraries(Engine01_Logger PUBLIC magic_enum project_options project_warnings)
