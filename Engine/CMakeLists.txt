PROJECT(Engine01)

add_subdirectory(Utils)
add_subdirectory(Logger)
add_subdirectory(Stream)
add_subdirectory(Math)
add_subdirectory(BaseObject)
add_subdirectory(Configuration)
add_subdirectory(ResourceSystem)
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
        src/Resource/ResourceBmp.cpp
        src/Resource/ResourceBmp.hpp
        src/States/State.hpp
        src/States/State.cpp
        include/Engine/System/InputEvent.hpp
        src/EngineConfiguration.cpp
        src/EngineConfiguration.hpp
        include/Engine/System/OutputScreen.hpp
        include/Engine/System/InputSystem.hpp
        src/States/Running_OnMap.cpp
        src/States/Running_OnMap.hpp
        src/Tileset.cpp
        src/Tileset.hpp
        include/Engine/CommonTypes.hpp
        src/States/LoadingMap.cpp
        src/States/LoadingMap.hpp
        src/configuration_error.cpp
        src/configuration_error.hpp
        src/Error.hpp
        src/Error.cpp
        include/Engine/System/RootStreamFactory.hpp)

target_include_directories(Engine01 PUBLIC include)
target_include_directories(Engine01 PRIVATE src)
target_link_libraries(Engine01 PUBLIC project_options project_warnings Engine01_Stream Engine01_Logger Engine01_Math Engine01_BaseObject Engine01_Configuration Engine01_ResourceSystem Engine01_Utils Engine01_ScriptSystem)

option(ENABLE_TESTING "Enable Test Builds" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()
