#include <Engine/ScriptSystem/ScriptEngine.hpp>
#include "LuaScriptEngine.hpp"

namespace e00::impl {
ScriptEngine::ScriptEngine() {
}

ScriptEngine::~ScriptEngine() {
}

std::unique_ptr<ScriptEngine> ScriptEngine::Create() {
  return std::unique_ptr<ScriptEngine>(new scripting::lua::LuaScriptEngine());
}

namespace scripting {
  const TypeInfo ProxyFunction::_end = TypeInfo();
}

}// namespace e00::impl
