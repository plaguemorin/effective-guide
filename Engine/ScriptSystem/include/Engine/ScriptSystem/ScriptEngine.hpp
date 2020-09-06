#pragma once

#include <string>
#include <type_traits>
#include <system_error>
#include <utility>
#include <functional>
#include <memory>
#include <map>

#include "detail/NativeFunctionT.hpp"

namespace e00::impl {
class ScriptEngine {
protected:
  ScriptEngine();

  virtual bool valid_fn_name(const std::string &fn_name) = 0;

  virtual void add_function(const std::string &fn_name, const std::shared_ptr<scripting::detail::ProxyFunction> &fn) = 0;

public:
  static std::unique_ptr<ScriptEngine> Create();

  virtual ~ScriptEngine();

  virtual const std::string &engine_name() const = 0;

  template<typename Fn>
  void register_function(const std::string &fn_name, Fn &&fn) {
    if (valid_fn_name(fn_name)) {
      add_function(fn_name, scripting::detail::make_function_t(fn, scripting::detail::FunctionSignature{ fn }));
    }
  }

  virtual std::error_code parse(const std::string &code) = 0;
};

}// namespace e00::impl
