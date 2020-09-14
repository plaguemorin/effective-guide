#include "NamedFunction.hpp"

#include <iostream>

#include "BoxedToLuaConverter.hpp"
#include "LuaToBoxedConverter.hpp"

namespace e00::impl::scripting::lua {

NamedFunction::NamedFunction(lua_State *L, std::string lua_fn_name, TypeInfo return_fn)
  : ProxyFunction({}, return_fn, -1),
    _fn_name(std::move(lua_fn_name)),
    _state(L) {
}

NamedFunction::~NamedFunction() {
}

BoxedValue NamedFunction::do_call(const FunctionParams &params) const {
  // Keep a record of how the stack is
  int stack_size = lua_gettop(_state);

  // Get a global of the function name
  lua_getglobal(_state, _fn_name.c_str());

  // This should already be true, but we never know if something went sideways at runtime
  if (!lua_isfunction(_state, -1)) {
    return {};
  }

  // Push our arguments into the stack
  for (const auto &p : params) {
    if (boxed_to_lua(_state, p) != 1) {
      std::cerr << "Error calling " << _fn_name << "\n"
                << "Don't know how to push arg\n";
      return BoxedValue();
    }
  }

  // Call the lua function and accept any number of return values
  if (lua_pcall(_state, (int)params.size(), LUA_MULTRET, 0)) {
    std::cerr << "Error calling " << _fn_name << "\n"
              << lua_error(_state) << "\n";
    return BoxedValue();
  }

  // Count number of returns
  int num_returns = lua_gettop(_state) - stack_size;
  std::cerr << "LUA Fn " << _fn_name << " returned " << num_returns << " returns";

  if (return_type().is_void()) {
    return BoxedValue();
  }

  auto r = lua_to_boxed_value(_state, -1, return_type());
  lua_pop(_state, 1);
  return r;
}
}// namespace e00::impl::scripting::lua
