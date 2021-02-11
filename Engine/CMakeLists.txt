PROJECT(Engine01)

add_subdirectory(Utils)
add_subdirectory(Logger)
add_subdirectory(Stream)
add_subdirectory(Math)
add_subdirectory(BaseObject)
add_subdirectory(Configuration)
#add_subdirectory(ResourceSystem)
add_subdirectory(ScriptSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01
        STATIC
        include/Engine.hpp
        src/Engine.cpp
        include/Engine/Math.hpp
        include/Engine/ThingSystem/Info.hpp
        include/Engine/ThingSystem/Thing.hpp
        src/ThingSystem/Thing.cpp
        include/Engine/ThingSystem/Manager.hpp
        src/ThingSystem/Manager.cpp
        include/Engine/ThingSystem/Actor.hpp
        src/ThingSystem/Actor.cpp
        include/Engine/ThingSystem/ComponentContainer.hpp
        src/ThingSystem/ComponentContainer.cpp
        include/Engine/System/InputEvent.hpp
        src/EngineConfiguration.cpp
        src/EngineConfiguration.hpp
        include/Engine/System/OutputScreen.hpp
        include/Engine/System/InputSystem.hpp
        include/Engine/CommonTypes.hpp
        src/configuration_error.cpp
        src/configuration_error.hpp
        src/Error.hpp
        src/Error.cpp
        include/Engine/Resource.hpp
        src/Resource.cpp
        include/Engine/Resources/ResourcePtr.hpp
        include/Engine/Resources/Bitmap.hpp
        include/Engine/Resources/Map.hpp
        include/Engine/Resources/Sprite.hpp
        include/Engine/Resources/Tileset.hpp
        src/Resources/Bitmap.cpp
        src/Resources/Map.cpp
        src/Resources/Sprite.cpp
        src/Resources/Tileset.cpp
        include/Engine/System/RootStreamFactory.hpp
        include/Engine/Menu.hpp
        src/MapRender.cpp
        src/MapRender.hpp)

target_include_directories(Engine01 PUBLIC include)
target_include_directories(Engine01 PRIVATE src)
target_link_libraries(Engine01 PUBLIC project_options project_warnings Engine01_Stream Engine01_Logger Engine01_Math Engine01_BaseObject Engine01_Configuration Engine01_Utils Engine01_ScriptSystem lodepng)

option(ENABLE_TESTING "Enable Test Builds" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()
