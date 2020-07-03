PROJECT(engine_lodepng)

add_library(lodepng STATIC src/LODEPNG.CPP include/LODEPNG.H)
target_include_directories(lodepng PUBLIC include)
target_link_libraries(lodepng PUBLIC project_options)
