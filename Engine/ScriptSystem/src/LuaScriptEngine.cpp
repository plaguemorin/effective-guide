#include "LuaScriptEngine.hpp"
#include "Lua/TrampolineData.hpp"
#include "Lua/LuaToNativeTrampoline.hpp"
#include "Lua/NamedFunction.hpp"

#include <iostream>

namespace e00::impl::scripting::lua {
LuaScriptEngine::LuaScriptEngine()
  : ScriptEngine(),
    _state(luaL_newstate()) {
  luaL_openlibs(_state);
}

LuaScriptEngine::~LuaScriptEngine() {
  lua_close(_state);
}

bool LuaScriptEngine::valid_fn_name(const std::string &fn_name) {
  auto it = _registered_fns.find(fn_name);
  return it == _registered_fns.end();
}

void LuaScriptEngine::add_function(const std::string &fn_name, std::unique_ptr<ProxyFunction> &&fn) {
  lua_pushlightuserdata(_state, new TrampolineData(this, fn, fn_name));
  lua_pushcclosure(_state, &lua_trampoline, 1);
  lua_setglobal(_state, fn_name.c_str());

  _registered_fns.insert(std::make_pair(fn_name, std::move(fn)));
}

void LuaScriptEngine::add_variable(const std::string &var_name, scripting::BoxedValue val) {
  (void)var_name;
  (void)val;

  // Is this a pointer ?
  /*  if (val.is_pointer()) {
    auto **lpudd = static_cast<LuaPointerUserDataData **>(lua_newuserdata(_state, sizeof(LuaPointerUserDataData *)));

    *lpudd = new LuaPointerUserDataData;

    lua_newtable(_state);
    lua_pushcfunction(_state, &LuaPointerUserDataDataIndex);
    lua_setfield(_state, -2, "__index");
    lua_pushcfunction(_state, &LuaPointerUserDataDataTostring);
    lua_setfield(_state, -2, "__tostring");
    lua_pushcfunction(_state, &LuaPointerUserDataDataGC);
    lua_setfield(_state, -2, "__gc");
    lua_setmetatable(_state, -2);
  }*/
}

std::error_code LuaScriptEngine::parse(const std::string &code) {
  luaL_loadstring(_state, code.c_str());
  lua_pcall(_state, 0, 0, 0);

  return {};
}
std::unique_ptr<scripting::ProxyFunction> LuaScriptEngine::get_function(const std::string &fn_name, scripting::TypeInfo preferred_return_type) {
  return std::unique_ptr<scripting::ProxyFunction>(
    new NamedFunction(_state, fn_name, preferred_return_type));
}
}// namespace e00::impl::scripting::lua
