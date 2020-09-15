#pragma once

#include <Engine/ScriptSystem/BoxedValue.hpp>

#include "Lua.hpp"

/**
 * Convert to a specific type
 *
 * @param L
 * @param n
 * @param info
 * @return
 */
e00::impl::scripting::BoxedValue lua_to_boxed_value(lua_State *L, int n, const e00::impl::scripting::TypeInfo &info);

/**
 * Convert to closest matching type
 *
 * @param L
 * @param n
 * @return
 */
e00::impl::scripting::BoxedValue lua_to_boxed_value_guess(lua_State *L, int n);
