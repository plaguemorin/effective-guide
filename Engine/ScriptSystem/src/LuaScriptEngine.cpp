#include "LuaScriptEngine.hpp"

#include <iostream>

constexpr auto context_global_var_name = "__ctx";

namespace {
e00::impl::scripting::detail::BoxedValue lua_to_boxed_value(lua_State *L, int n) {
  switch (lua_type(L, n)) {
    case LUA_TNIL:
      break;

    case LUA_TNUMBER:
      return e00::impl::scripting::detail::BoxedValue(lua_tonumber(L, n));

    case LUA_TBOOLEAN:
      return e00::impl::scripting::detail::BoxedValue(lua_toboolean(L, n));

    case LUA_TSTRING:
      return e00::impl::scripting::detail::BoxedValue(std::string_view(lua_tostring(L, n)));

    case LUA_TTABLE:
      break;

    case LUA_TFUNCTION:
      break;

    case LUA_TUSERDATA:
      break;

    case LUA_TTHREAD:
      break;

    case LUA_TLIGHTUSERDATA:
      break;
  }
}

extern "C" int lua_trampoline(lua_State *L) {
  auto ctx = static_cast<e00::impl::scripting::lua::LuaScriptEngine *>(lua_touserdata(L, lua_upvalueindex(1)));
  auto fn = static_cast<e00::impl::scripting::detail::ProxyFunction *>(lua_touserdata(L, lua_upvalueindex(2)));
  auto fn_name = lua_tolstring(L, lua_upvalueindex(3), nullptr);

  if (!fn) {
    // oops!
    return 0;
  }

  auto args_count = lua_gettop(L);
  std::cout << "Called " << fn_name << ". has " << fn->parameter_count() << " parameters. got " << args_count << " from script";

  // Count arguments
  if (args_count != fn->parameter_count()) {
    std::cerr << "Wrong number of arguments\n";
    return 0;
  }

  //
  std::vector<e00::impl::scripting::detail::BoxedValue> values;
  values.resize(args_count);
  for (int i = 0; i <= fn->parameter_count(); i++) {
    values[i] = lua_to_boxed_value(L, i + 1);
  }

  return 0;
}
}// namespace

namespace e00::impl::scripting::lua {
LuaScriptEngine::LuaScriptEngine()
  : ScriptEngine(),
    _state(luaL_newstate()) {
}

LuaScriptEngine::~LuaScriptEngine() {
  lua_close(_state);
}

bool LuaScriptEngine::valid_fn_name(const std::string &fn_name) {
  (void)fn_name;
  return true;
}

void LuaScriptEngine::add_function(const std::string &fn_name, const std::shared_ptr<scripting::detail::ProxyFunction> &fn) {
  _registered_fns.insert(std::make_pair(fn_name, fn));

  lua_pushlightuserdata(_state, this);
  lua_pushlightuserdata(_state, fn.get());
  lua_pushstring(_state, fn_name.c_str());
  lua_pushcclosure(_state, &lua_trampoline, 3);
  lua_setglobal(_state, fn_name.c_str());
}

std::error_code LuaScriptEngine::parse(const std::string &code) {
  luaL_loadstring(_state, code.c_str());
  lua_pcall(_state, 0, 0, 0);

  return {};
}


}// namespace e00::impl::scripting::lua
