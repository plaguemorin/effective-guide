PROJECT(Engine01_ResourceSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_ResourceSystem STATIC
        include/Engine/ResourceSystem/Resource.hpp
        src/Resource.cpp
        include/Engine/ResourceSystem/Resources/Bitmap.hpp
        src/Bitmap.cpp
        include/Engine/ResourceSystem/Resources/Map.hpp
        src/Map.cpp
        include/Engine/ResourceSystem/Resources/Sprite.hpp
        src/Sprite.cpp
        include/Engine/ResourceSystem/Manager.hpp
        src/Manager.cpp
        include/Engine/ResourceSystem/Error.hpp
        src/Error.cpp
        src/ResourcePackConfiguration.cpp
        src/ResourcePackConfiguration.hpp
        include/Engine/ResourceSystem/Info/Type.hpp
        include/Engine/ResourceSystem/Info/MapPosition.hpp
        include/Engine/ResourceSystem/Info/Map.hpp
        include/Engine/ResourceSystem/Info/Actor.hpp
        include/Engine/ResourceSystem/Info/Portal.hpp
        include/Engine/ResourceSystem/Info/Sprite.hpp
        include/Engine/ResourceSystem/Info/Tileset.hpp
        include/Engine/ResourceSystem/Info/Bitmap.hpp
        src/Info.hpp
        src/Info.cpp
        src/PackIndexMaker.cpp
        src/PackIndexMaker.hpp
        src/ResourceSectionStringConvert.hpp
        src/PackIndex.cpp
        include/Engine/ResourceSystem/PackIndex.hpp
        src/LoaderMap.cpp
        src/LoaderMap.hpp
        src/base64.hpp
        include/Engine/ResourceSystem/ResourcePtr.hpp src/ResourcePtr.cpp)

target_include_directories(Engine01_ResourceSystem PRIVATE src)

target_include_directories(Engine01_ResourceSystem PUBLIC include)
target_link_libraries(Engine01_ResourceSystem PUBLIC project_options project_warnings Engine01_Utils Engine01_Logger Engine01_BaseObject Engine01_Math Engine01_Stream Engine01_Configuration)
