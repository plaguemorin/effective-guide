# automatically enable catch2 to generate ctest targets
add_library(script_catch_main STATIC catch_main.cpp)
target_include_directories(script_catch_main PUBLIC ../../../lib)

add_executable(script_tests tests.cpp)
target_include_directories(script_tests PRIVATE ../src)
target_link_libraries(script_tests PRIVATE project_warnings project_options
        Engine01_ScriptSystem catch_main)

