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
  std::map<std::string, std::shared_ptr<scripting::detail::ProxyFunction>> _registered_fns;
  lua_State *_state;

public:
  LuaScriptEngine();

  ~LuaScriptEngine() override;

  const std::string &engine_name() const override { return lua_engine_name; }

protected:
  bool valid_fn_name(const std::string &fn_name) override;
  void add_function(const std::string &fn_name, const std::shared_ptr<scripting::detail::ProxyFunction> &fn) override;

public:
  std::error_code parse(const std::string &code) override;
};
}// namespace e00::impl::scripting::lua
