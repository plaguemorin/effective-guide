#include "BoxedToLuaConverter.hpp"
#include <Engine/ScriptSystem/BoxedCast.hpp>

#include <string>
#include <string_view>

using namespace e00::impl::scripting;

int boxed_to_lua(lua_State *L, const BoxedValue &boxed_rv) {
  if (boxed_rv.is_void()) {
    return 0;
  }

  bool matched =//
    try_cast<bool>(boxed_rv, [&L](bool a) { lua_pushboolean(L, (a ? 1 : 0)); }) ||//
    try_cast<int>(boxed_rv, [&L](int a) { lua_pushinteger(L, a); }) ||//
    try_cast<long>(boxed_rv, [&L](long a) { lua_pushinteger(L, a); }) ||//
    try_cast<float>(boxed_rv, [&L](float a) { lua_pushnumber(L, a); }) ||//
    try_cast<std::string>(boxed_rv, [&L](const std::string &str) { lua_pushlstring(L, str.c_str(), str.size()); }) ||//
    try_cast<std::string_view>(boxed_rv, [&L](const std::string_view &str) { lua_pushlstring(L, str.data(), str.size()); });//

  if (!matched) {
    if (boxed_rv.is_class()) {
    }
  }

  return matched ? 1 : 0;
}
