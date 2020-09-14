#pragma once

#include <Engine/ScriptSystem/ScriptEngine.hpp>
#include <system_error>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace {
const std::string lua_engine_name = "Lua";
}

namespace e00::impl::scripting::lua {
class LuaScriptEngine : public ScriptEngine {
  std::map<std::string, std::unique_ptr<scripting::ProxyFunction>> _registered_fns;
  lua_State *_state;

public:
  LuaScriptEngine();

  ~LuaScriptEngine() override;

  const std::string &engine_name() const override { return lua_engine_name; }

protected:
  bool valid_fn_name(const std::string &fn_name) override;
  void add_function(const std::string &fn_name, std::unique_ptr<scripting::ProxyFunction> &&fn) override;
  void add_variable(const std::string &var_name, scripting::BoxedValue val) override;

public:
  std::error_code parse(const std::string &code) override;

  std::unique_ptr<scripting::ProxyFunction> get_function(const std::string &fn_name, scripting::TypeInfo preferred_return_type) override;
};
}// namespace e00::impl::scripting::lua
