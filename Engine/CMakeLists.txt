PROJECT(Engine01)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01
        STATIC
        include/Engine.hpp src/Engine.cpp
        include/Engine/Math.hpp
        include/Engine/Resource/Bitmap.hpp src/Resource/Bitmap.cpp
        include/Engine/ThingSystem/Info.hpp
        include/Engine/ThingSystem/Thing.hpp src/ThingSystem/Thing.cpp
        include/Engine/ThingSystem/Manager.hpp src/ThingSystem/Manager.cpp
        include/Engine/ThingSystem/Actor.hpp src/ThingSystem/Actor.cpp
        include/Engine/ThingSystem/ComponentContainer.hpp src/ThingSystem/ComponentContainer.cpp
        include/Engine/TypedObject.hpp
        include/Engine/Resource/Resource.hpp src/Resource/Resource.cpp
        include/Engine/NamedObject.hpp src/NamedObject.cpp
        include/Engine/TypedObjectContainer.hpp
        src/INIReader/INIReader.hpp
        src/Resource/ResourceBmp.cpp
        src/Resource/ResourceBmp.hpp
        include/Engine/ResourceStream.hpp
        src/States/State.hpp src/States/State.cpp
        include/Engine/System/InputEvent.hpp
        src/INIReader/StreamIniReader.cpp
        src/INIReader/StreamIniReader.hpp
        src/EngineConfiguration.cpp
        src/EngineConfiguration.hpp
        include/Engine/System/OutputScreen.hpp
        include/Engine/System/InputSystem.hpp
        include/Engine/System/ResourceStreamLoader.hpp
        src/Logger.hpp
        include/Engine/System/LoggerSink.hpp
        src/Logger.cpp src/StrFormat/Format.hpp include/Engine/RuntimeConfig/Entry.hpp include/Engine/RuntimeConfig/ParserListener.hpp src/States/Running_OnMap.cpp src/States/Running_OnMap.hpp src/Resource/Map.cpp src/Resource/Map.hpp src/Tileset.cpp src/Tileset.hpp src/Loaders/MapConfigParser.cpp src/Loaders/MapConfigParser.hpp src/string_view_utils.hpp include/Engine/CommonTypes.hpp src/string_view_utils.cpp src/States/LoadingMap.cpp src/States/LoadingMap.hpp src/configuration_error.cpp src/configuration_error.hpp include/Engine/Resource/SharedResource.hpp src/ResourceConfiguration.hpp src/ResourceManager.cpp src/ResourceManager.hpp src/Error.hpp src/Error.cpp src/Loaders/ResourcePackConfiguration.cpp src/Loaders/ResourcePackConfiguration.hpp)

target_include_directories(Engine01 PUBLIC include)
target_compile_options(Engine01 PUBLIC -fno-rtti)
target_link_libraries(Engine01 PUBLIC magic_enum project_options project_warnings)

option(ENABLE_TESTING "Enable Test Builds" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()
