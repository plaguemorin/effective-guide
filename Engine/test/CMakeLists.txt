# automatically enable catch2 to generate ctest targets
add_library(catch_main STATIC catch_main.cpp)
target_include_directories(catch_main PUBLIC ../../lib)

add_executable(tests tests.cpp)
target_include_directories(tests PRIVATE ../src)
target_link_libraries(tests PRIVATE project_warnings project_options
        Engine01 catch_main)

