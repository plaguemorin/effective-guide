#pragma once

#include <string_view>

#include <Engine/ScriptSystem/ProxyFunction.hpp>
#include "Lua.hpp"

namespace e00::impl::scripting::lua {
class RefFunction : public ProxyFunction {
  lua_State *_l;
  int _ref = 0;

public:
  RefFunction(lua_State *L, int ref_)
    : ProxyFunction({}, {}, -1),
      _l(L),
      _ref(ref_) {}

  ~RefFunction() override {
    luaL_unref(_l, LUA_REGISTRYINDEX, _ref);
  }

protected:
  BoxedValue do_call(const FunctionParams &params) const override {
    return BoxedValue();
  }
};
}// namespace e00::impl::scripting::lua
