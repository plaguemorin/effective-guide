PROJECT(Engine01_ScriptSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_ScriptSystem STATIC
        src/ScriptEngine.cpp
        include/Engine/ScriptSystem/BoxedValue.hpp
        include/Engine/ScriptSystem/TypeInfo.hpp
        include/Engine/ScriptSystem/FunctionParams.hpp
        include/Engine/ScriptSystem/ProxyFunction.hpp
        include/Engine/ScriptSystem/detail/FunctionSignature.hpp
        include/Engine/ScriptSystem/detail/HandleReturn.hpp
        include/Engine/ScriptSystem/detail/Utils.hpp
        include/Engine/ScriptSystem/detail/NativeFunctionT.hpp
        include/Engine/ScriptSystem/detail/CallUtils.hpp
        include/Engine/ScriptSystem/detail/AttributeAccess.hpp
        include/Engine/ScriptSystem/detail/CastHelpers.hpp
        include/Engine/ScriptSystem/BoxedCast.hpp
        include/Engine/ScriptSystem/ScriptEngine.hpp
        src/LuaScriptEngine.cpp
        src/LuaScriptEngine.hpp
        src/Lua/RefFunction.cpp
        src/Lua/RefFunction.hpp
        src/Lua/Lua.hpp
        src/Lua/TrampolineData.hpp
        src/Lua/LuaToNativeTrampoline.cpp
        src/Lua/LuaToNativeTrampoline.hpp
        src/Lua/LuaToBoxedConverter.cpp
        src/Lua/LuaToBoxedConverter.hpp
        src/Lua/BoxedToLuaConverter.cpp
        src/Lua/BoxedToLuaConverter.hpp
        src/Lua/NamedFunction.cpp
        src/Lua/NamedFunction.hpp
        src/Lua/LuaProxyFunction.cpp
        src/Lua/LuaProxyFunction.hpp
        src/Lua/UserDataHolder.cpp
        src/Lua/UserDataHolder.hpp
        src/Lua/Error.cpp
        src/Lua/Error.hpp)


target_include_directories(Engine01_ScriptSystem PRIVATE src)
target_include_directories(Engine01_ScriptSystem PUBLIC include)
target_link_libraries(Engine01_ScriptSystem PUBLIC project_options project_warnings Engine01_Logger Engine01_Stream lua)

add_subdirectory(test)
