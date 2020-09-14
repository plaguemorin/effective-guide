#pragma once

#include <string>
#include <Engine/ScriptSystem/ProxyFunction.hpp>

#include "Lua.hpp"

namespace e00::impl::scripting::lua {

class NamedFunction final : public ProxyFunction {
  const std::string _fn_name;
  lua_State *_state;

protected:
  BoxedValue do_call(const FunctionParams &params) const override;

public:
  NamedFunction(lua_State* L, std::string lua_fn_name, TypeInfo return_fn);

  ~NamedFunction() override;
};
}// namespace e00::impl::scripting::lua
