#pragma once

#include "TypeInfo.hpp"
#include "BoxedValue.hpp"
#include "FunctionParams.hpp"

namespace e00::impl::scripting::detail {
/**
 * A callable method
 * This is the only function the script engine impl can deal with
 */
class ProxyFunction {
  int _param_count;
  std::vector<TypeInfo> _params;
  TypeInfo _return_type;

protected:
  explicit ProxyFunction(std::vector<TypeInfo> types, TypeInfo return_type)
    : _param_count(types.size()),
      _params(std::move(types)),
      _return_type(return_type) {
  }

  /* Actually do the call */
  virtual BoxedValue do_call(const FunctionParams &params) const = 0;

public:
  virtual ~ProxyFunction() = default;

  const std::vector<TypeInfo> &parameters() const { return _params; }

  const TypeInfo &return_type() const { return _return_type; }

  virtual bool is_attribute_function() const noexcept { return false; }

  int parameter_count() const { return _param_count; }

  BoxedValue operator()(const FunctionParams &params) const {
    if (_param_count < 0 || size_t(_param_count) == params.size()) {
      return do_call(params);
    }

    return BoxedValue();
  }
};

}// namespace e00::impl::scripting::detail
