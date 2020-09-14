#include "BoxedToLuaConverter.hpp"

using namespace e00::impl::scripting;

int boxed_to_lua(lua_State *L, const BoxedValue &boxed_rv) {
  if (boxed_rv.is_void()) {
    return 0;
  }

  bool matched =
    boxed_rv.matches<bool>([&L](bool a) { lua_pushboolean(L, (a ? 1 : 0)); }) |//
    boxed_rv.matches<int>([&L](int a) { lua_pushinteger(L, a); }) |//
    boxed_rv.matches<long>([&L](long a) { lua_pushinteger(L, a); }) |//
    boxed_rv.matches<float>([&L](float a) { lua_pushnumber(L, a); }) |//
    boxed_rv.matches<std::string>([&L](const std::string &str) { lua_pushlstring(L, str.c_str(), str.size()); }) |//
    boxed_rv.matches<std::string_view>([&L](const std::string_view &str) { lua_pushlstring(L, str.data(), str.size()); });//

  return matched ? 1 : 0;
}
