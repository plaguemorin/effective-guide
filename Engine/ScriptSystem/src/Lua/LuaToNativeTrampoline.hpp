#pragma once

#include <vector>
#include <Engine/ScriptSystem/ProxyFunction.hpp>
#include <Engine/ScriptSystem/BoxedValue.hpp>
#include "Lua.hpp"

extern "C" int lua_trampoline(lua_State *L);

std::vector<e00::impl::scripting::BoxedValue> extract_lua_arguments_for_proxyfunction(lua_State *L, const std::unique_ptr<e00::impl::scripting::ProxyFunction>& func);
