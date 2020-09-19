#include "LuaScriptEngine.hpp"
#include "Lua/TrampolineData.hpp"
#include "Lua/LuaToNativeTrampoline.hpp"
#include "Lua/NamedFunction.hpp"
#include "Lua/RefFunction.hpp"
#include "Lua/BoxedToLuaConverter.hpp"
#include "Lua/UserDataHolder.hpp"

#include <iostream>

namespace e00::impl::scripting::lua {
LuaScriptEngine::LuaScriptEngine()
  : ScriptEngine(),
    _state(luaL_newstate()),
    _bad_method(nullptr) {
  luaL_openlibs(_state);

  // Make our genetic metatable for userdata
  // The if is technically not required as there's no chance we re-execute this
  if (luaL_newmetatable(_state, UserDataHolder::MetaTableName)) {


    lua_pushcfunction(_state, &UserDataHolder::LuaGc);
    lua_setfield(_state, -2, "__gc");

    lua_pushlightuserdata(_state, this);
    lua_pushcclosure(_state, &UserDataHolder::LuaIndex, 1);
    lua_setfield(_state, -2, "__index");

    lua_pushcfunction(_state, &UserDataHolder::LuaToString);
    lua_setfield(_state, -2, "__tostring");

    lua_pop(_state, 1);
  }
}

LuaScriptEngine::~LuaScriptEngine() {
  lua_close(_state);
}

bool LuaScriptEngine::valid_fn_name(const std::string &fn_name) {
  // TODO: Validate name
  return !fn_name.empty();
}

void LuaScriptEngine::add_function(const std::string &fn_name, std::unique_ptr<ProxyFunction> &&fn) {
  if (fn->is_member()) {
    // This will be used for <something>:fn_name
    const auto type_id = fn->parameter(0);
    _methods[type_id.bare_id()].insert(std::make_pair(fn_name, std::move(fn)));
    return;
  }

  lua_pushlightuserdata(_state, new TrampolineData(this, std::move(fn), fn_name));
  lua_pushcclosure(_state, &lua_trampoline, 1);
  lua_setglobal(_state, fn_name.c_str());
}

void LuaScriptEngine::add_variable(const std::string &var_name, scripting::BoxedValue val) {
  auto ret = boxed_to_lua(_state, val);
  if (ret == 1) {
    lua_setglobal(_state, var_name.c_str());
  }
}

void LuaScriptEngine::add_type(const TypeInfo &type) {
  if (!type.is_class()) {
    return;
  }
}

std::error_code LuaScriptEngine::parse(const std::string &code) {
  luaL_loadstring(_state, code.c_str());
  lua_pcall(_state, 0, 0, 0);

  return {};
}
std::unique_ptr<scripting::ProxyFunction> LuaScriptEngine::get_function(const std::string &fn_name, scripting::TypeInfo preferred_return_type) {
  // Get a global of the function name
  lua_getglobal(_state, fn_name.c_str());

  // This should already be true, but we never know if something went sideways at runtime
  if (!lua_isfunction(_state, -1)) {
    return nullptr;
  }

  // take a ref to it
  return std::unique_ptr<scripting::ProxyFunction>(
    new RefFunction(fn_name, _state, luaL_ref(_state, LUA_REGISTRYINDEX), preferred_return_type));
}

const std::unique_ptr<scripting::ProxyFunction> &LuaScriptEngine::get_method_for_type(const TypeInfo& type, const std::string &method_name) const {
  const auto it = _methods.find(type.bare_id());
  if (it != _methods.end()) {
    const auto mit = it->second.find(method_name);
    if (mit != it->second.end()) {
      return mit->second;
    }
  }

  return _bad_method;
}
}// namespace e00::impl::scripting::lua
