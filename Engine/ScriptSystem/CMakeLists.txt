PROJECT(Engine01_ScriptSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Engine01_ScriptSystem STATIC
        src/ScriptEngine.cpp
        include/Engine/ScriptSystem/ScriptEngine.hpp
        src/LuaScriptEngine.cpp
        src/LuaScriptEngine.hpp
        include/Engine/ScriptSystem/detail/ProxyFunction.hpp
        include/Engine/ScriptSystem/detail/BoxedValue.hpp
        include/Engine/ScriptSystem/detail/FunctionParams.hpp
        include/Engine/ScriptSystem/detail/FunctionSignature.hpp include/Engine/ScriptSystem/detail/HandleReturn.hpp include/Engine/ScriptSystem/detail/Utils.hpp include/Engine/ScriptSystem/detail/NativeFunctionT.hpp include/Engine/ScriptSystem/detail/CallUtils.hpp include/Engine/ScriptSystem/detail/AttributeAccess.hpp include/Engine/ScriptSystem/detail/TypeInfo.hpp)


target_include_directories(Engine01_ScriptSystem PRIVATE src)
target_include_directories(Engine01_ScriptSystem PUBLIC include)
target_link_libraries(Engine01_ScriptSystem PUBLIC project_options project_warnings Engine01_Logger lua)

add_subdirectory(test)
