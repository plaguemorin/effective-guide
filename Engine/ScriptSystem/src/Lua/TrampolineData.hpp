#pragma once

#include <memory>
#include <string>

#include "Lua.hpp"
#include <Engine/ScriptSystem/ProxyFunction.hpp>

namespace e00::impl::scripting::lua {
class LuaScriptEngine;

class TrampolineData {
  LuaScriptEngine *_engine;
  ProxyFunction *_fn;
  std::string _lua_name;

public:
  TrampolineData(LuaScriptEngine *engine, ProxyFunction *fn, std::string name)
    : _engine(engine), _fn(fn), _lua_name(std::move(name)) {}

  TrampolineData(LuaScriptEngine *engine, const std::unique_ptr<ProxyFunction> &fn, std::string name)
    : _engine(engine), _fn(fn.get()), _lua_name(std::move(name)) {}

  ProxyFunction *fn() const { return _fn; }
};
}// namespace e00::impl::scripting::lua
