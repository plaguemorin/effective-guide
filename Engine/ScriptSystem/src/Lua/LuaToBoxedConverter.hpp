#pragma once

#include <Engine/ScriptSystem/BoxedValue.hpp>

#include "Lua.hpp"

e00::impl::scripting::BoxedValue lua_to_boxed_value(lua_State *L, int n, const e00::impl::scripting::TypeInfo &info);
