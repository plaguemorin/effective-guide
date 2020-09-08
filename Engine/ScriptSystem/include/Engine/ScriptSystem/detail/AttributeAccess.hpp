#pragma once

#include "../ProxyFunction.hpp"
#include "HandleReturn.hpp"

namespace e00::impl::scripting::detail {
template<typename T, typename Class>
class Attribute_Access final : public ProxyFunction {
public:
  explicit Attribute_Access(T Class::*t_attr)
    : ProxyFunction(1),
      m_attr(t_attr) {
  }

  bool is_attribute_function() const noexcept override { return true; }

protected:
  BoxedValue do_call(const FunctionParams &params) const override {
    const BoxedValue &bv = params[0];
    if (bv.is_const()) {
      const Class *o = CastHelper<const Class *>::cast(bv);
      return do_call_impl<T>(o);
    } else {
      Class *o = CastHelper<Class *>::cast(bv);
      return do_call_impl<T>(o);
    }
  }

private:
  template<typename Type>
  auto do_call_impl(Class *o) const {
    if constexpr (std::is_pointer<Type>::value) {
      return detail::Handle_Return<Type>::handle(o->*m_attr);
    } else {
      return detail::Handle_Return<typename std::add_lvalue_reference<Type>::type>::handle(o->*m_attr);
    }
  }

  template<typename Type>
  auto do_call_impl(const Class *o) const {
    if constexpr (std::is_pointer<Type>::value) {
      return detail::Handle_Return<const Type>::handle(o->*m_attr);
    } else {
      return detail::Handle_Return<typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type>::handle(o->*m_attr);
    }
  }

//
//  static std::vector<Type_Info> param_types() {
//    return { user_type<T>(), user_type<Class>() };
//  }
//
//  std::vector<Type_Info> m_param_types{ user_type<T>(), user_type<Class>() };
  T Class::*m_attr;
};

}// namespace e00::impl::scripting::detail
