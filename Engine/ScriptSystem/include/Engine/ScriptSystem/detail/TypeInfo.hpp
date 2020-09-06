#pragma once

#include <type_traits>
namespace e00::impl::scripting::detail {
struct TypeInfo {
  constexpr TypeInfo() = default;

  constexpr TypeInfo(const bool t_is_const, const bool t_is_reference, const bool t_is_pointer, const bool t_is_void, const bool t_is_arithmetic) noexcept
    : _flags((static_cast<unsigned int>(t_is_const) << is_const_flag)
             + (static_cast<unsigned int>(t_is_reference) << is_reference_flag)
             + (static_cast<unsigned int>(t_is_pointer) << is_pointer_flag)
             + (static_cast<unsigned int>(t_is_void) << is_void_flag)
             + (static_cast<unsigned int>(t_is_arithmetic) << is_arithmetic_flag)) {
  }

  constexpr TypeInfo(const TypeInfo &rhs) noexcept
    : _flags(rhs._flags) {}

  bool is_const() const noexcept { return (_flags & (1u << is_const_flag)) != 0; }
  bool is_reference() const noexcept { return (_flags & (1u << is_reference_flag)) != 0; }
  bool is_void() const noexcept { return (_flags & (1u << is_void_flag)) != 0; }
  bool is_arithmetic() const noexcept { return (_flags & (1u << is_arithmetic_flag)) != 0; }
  bool is_undef() const noexcept { return (_flags & (1u << is_undef_flag)) != 0; }
  bool is_pointer() const noexcept { return (_flags & (1u << is_pointer_flag)) != 0; }

  TypeInfo& operator=(const TypeInfo& rhs) = default;

private:
  constexpr static unsigned int is_const_flag = 0;
  constexpr static unsigned int is_reference_flag = 1;
  constexpr static unsigned int is_pointer_flag = 2;
  constexpr static unsigned int is_void_flag = 3;
  constexpr static unsigned int is_arithmetic_flag = 4;
  constexpr static unsigned int is_undef_flag = 5;
  unsigned int _flags = (1u << is_undef_flag);
};

template<typename T>
TypeInfo user_type() {
  return TypeInfo(std::is_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value,
    std::is_reference<T>::value,
    std::is_pointer<T>::value,
    std::is_void<T>::value,
    (std::is_arithmetic<T>::value || std::is_arithmetic<typename std::remove_reference<T>::type>::value)
      && !std::is_same<typename std::remove_const<typename std::remove_reference<T>::type>::type, bool>::value);
}

template<typename T>
TypeInfo user_type(const T &) {
  return user_type<T>();
}


}// namespace e00::impl::scripting::detail
