PROJECT(Engine01)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01
        STATIC
        include/Engine.hpp                          src/Engine.cpp
        include/Engine/System.hpp
        include/Engine/Math.hpp                     src/Math.cpp
        include/Engine/Bitmap.hpp                   src/Bitmap.cpp
        include/Engine/ThingSystem/Info.hpp
        include/Engine/ThingSystem/Thing.hpp        src/ThingSystem/Thing.cpp
        include/Engine/Map.hpp                      src/Map.cpp
        include/Engine/TickCommand.hpp
        include/Engine/TileSet.hpp                  src/TileSet.cpp
        include/Engine/ThingSystem/Manager.hpp      src/ThingSystem/Manager.cpp
        include/Engine/ThingSystem/Actor.hpp        src/ThingSystem/Actor.cpp
        include/Engine/ThingSystem/Enemy.hpp        src/ThingSystem/Enemy.cpp
        include/Engine/ThingSystem/ComponentContainer.hpp src/ThingSystem/ComponentContainer.cpp
        include/Engine/TypedObject.hpp
        include/Engine/Resource/Sprite.hpp          src/Resource/Sprite.cpp
        include/Engine/Resource/Resource.hpp        src/Resource/Resource.cpp
        include/Engine/NamedObject.hpp              src/NamedObject.cpp include/Engine/TypedObjectContainer.hpp)

target_include_directories(Engine01 PUBLIC include)
target_compile_options(Engine01 PUBLIC -fno-rtti)
