#pragma once

#include "TypeInfo.hpp"

namespace e00::impl::scripting::detail {
/**
 * Wrapper for holding any support C++ type
 * The script engine only understands BoxedValues for values
 */
struct BoxedValue {
  /// used for explicitly creating a "void" object
  struct VoidType {};

  /// Basic Boxed_Value constructor
  template<typename T, typename = std::enable_if_t<!std::is_same_v<BoxedValue, std::decay_t<T>>>>
  explicit BoxedValue(T &&t, bool t_return_value = false)
    //  : m_data(Object_Data::get(std::forward<T>(t), t_return_value))
    : _info(user_type<T>()) {
    (void)t;
    (void)t_return_value;
  }

  /// Unknown-type constructor
  BoxedValue() : _info(user_type<void>()) {}

  BoxedValue(BoxedValue &&) = default;
  BoxedValue &operator=(BoxedValue &&) = default;
  BoxedValue(const BoxedValue &) = default;
  BoxedValue &operator=(const BoxedValue &) = default;

  bool is_const() const { return _info.is_const(); }

private:
  TypeInfo _info;
};

inline BoxedValue void_var() {
  static const auto v = BoxedValue(BoxedValue::VoidType());
  return v;
}

////
//// Casting helpers
////
template<typename T>
struct Cast_Helper_Inner {};

template<>
struct Cast_Helper_Inner<int> {
  static int cast(const BoxedValue &ob) {
    (void)ob;
    return 0;
  }
};

/// The exposed Cast_Helper object that by default just calls the Cast_Helper_Inner
template<typename T>
struct CastHelper {
  static decltype(auto) cast(const BoxedValue &ob) {
    return (Cast_Helper_Inner<T>::cast(ob));
  }
};
}// namespace e00::impl::scripting::detail
