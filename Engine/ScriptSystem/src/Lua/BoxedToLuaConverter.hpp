#pragma once

#include <Engine/ScriptSystem/BoxedValue.hpp>

#include "Lua.hpp"

int boxed_to_lua(lua_State *L, const e00::impl::scripting::BoxedValue &boxed_rv);
