#pragma once

#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <memory>

namespace e00::impl::scripting {
using type_id_no_rtti = std::ptrdiff_t;
class ProxyFunction;

namespace detail {
  template<typename T>
  struct Bare_Type {
    using type = typename std::remove_cv<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::type;
  };

  // Make a unique pointer for types; this gets us somewhat of an RTTI
  template<typename T>
  type_id_no_rtti make_type_id() {
    static char uniqueT = {};
    return reinterpret_cast<type_id_no_rtti>(&uniqueT);
  }

  template<typename T>
  const char* make_name() {
    return {};
  }

  // Don't touch this!
  // clang-format off
  // @formatter:off
#define MAKE_TYPE_INFO_FOR_INTERNAL2(TYPE, NUMBER) \
      template<> constexpr type_id_no_rtti make_type_id<TYPE>()                           { return (type_id_no_rtti)(NUMBER); } \
      template<> constexpr const char* make_name<TYPE>()                             { return #TYPE; }

#define MAKE_TYPE_ID_FOR_INTERNAL(TYPE, START_AT) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(TYPE,                            (START_AT) + 0) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(const TYPE,                      (START_AT) + 1) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(TYPE *,                          (START_AT) + 2) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(const TYPE *,                    (START_AT) + 3) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(TYPE &,                          (START_AT) + 4) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(const TYPE &,                    (START_AT) + 5) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(std::unique_ptr<TYPE>,           (START_AT) + 6) \
      MAKE_TYPE_INFO_FOR_INTERNAL2(const std::unique_ptr<TYPE> &,   (START_AT) + 7)

#define MAKE_TYPE_ID_FOR(TYPE)  MAKE_TYPE_ID_FOR_INTERNAL(TYPE, __COUNTER__ * 8)
  // @formatter:on
  // clang-format on
  // Carry on formatting

  // Constexpr some basic types
  MAKE_TYPE_ID_FOR(char)
  MAKE_TYPE_ID_FOR(unsigned char)
  MAKE_TYPE_ID_FOR(int)
  MAKE_TYPE_ID_FOR(unsigned int)
  MAKE_TYPE_ID_FOR(long)
  MAKE_TYPE_ID_FOR(unsigned long)
  MAKE_TYPE_ID_FOR(float)
  MAKE_TYPE_ID_FOR(double)

  // Constexpr some well used types
  MAKE_TYPE_ID_FOR(std::string)
  MAKE_TYPE_ID_FOR(std::string_view)
  MAKE_TYPE_ID_FOR(ProxyFunction)

#undef MAKE_TYPE_ID_FOR
#undef MAKE_TYPE_ID_FOR_INTERNAL
}// namespace detail

struct TypeInfo {
  constexpr TypeInfo() = default;

  constexpr TypeInfo(const bool t_is_const, const bool t_is_reference, const bool t_is_pointer, const bool t_is_void, const bool t_is_arithmetic, const bool t_is_class, type_id_no_rtti t_type, type_id_no_rtti t_bare, const char* t_name) noexcept
    : _flags((static_cast<unsigned int>(t_is_const) << is_const_flag)
             + (static_cast<unsigned int>(t_is_reference) << is_reference_flag)
             + (static_cast<unsigned int>(t_is_pointer) << is_pointer_flag)
             + (static_cast<unsigned int>(t_is_void) << is_void_flag)
             + (static_cast<unsigned int>(t_is_arithmetic) << is_arithmetic_flag)
             + (static_cast<unsigned int>(t_is_class) << is_class_flag)),
      _type(t_type),
      _bare(t_bare),
      _name(t_name) {
  }

  constexpr TypeInfo(const TypeInfo &rhs) noexcept = default;

  constexpr bool is_const() const noexcept { return (_flags & (1u << is_const_flag)) != 0; }
  constexpr bool is_reference() const noexcept { return (_flags & (1u << is_reference_flag)) != 0; }
  constexpr bool is_void() const noexcept { return (_flags & (1u << is_void_flag)) != 0; }
  constexpr bool is_arithmetic() const noexcept { return (_flags & (1u << is_arithmetic_flag)) != 0; }
  constexpr bool is_undef() const noexcept { return (_flags & (1u << is_undef_flag)) != 0; }
  constexpr bool is_pointer() const noexcept { return (_flags & (1u << is_pointer_flag)) != 0; }
  constexpr bool is_class() const noexcept { return (_flags & (1u << is_class_flag)) != 0; }

  constexpr bool operator==(const TypeInfo &rhs) const noexcept {
    return _type == rhs._type;
  }

  constexpr bool operator!=(const TypeInfo &rhs) const noexcept {
    return _type != rhs._type;
  }

  constexpr bool bare_equal_type_info(const TypeInfo &ti) const noexcept {
    return _bare == ti._bare;
  }


  constexpr std::string_view name() const noexcept { return _name; }

private:
  constexpr static unsigned int is_const_flag = 0;
  constexpr static unsigned int is_reference_flag = 1;
  constexpr static unsigned int is_pointer_flag = 2;
  constexpr static unsigned int is_void_flag = 3;
  constexpr static unsigned int is_arithmetic_flag = 4;
  constexpr static unsigned int is_class_flag = 5;
  constexpr static unsigned int is_undef_flag = 8;
  unsigned int _flags = (1u << is_undef_flag);
  type_id_no_rtti _type = {};
  type_id_no_rtti _bare = {};
  const char* _name = {};
};

namespace detail {
  template<typename T>
  struct GetTypeInfo {
    constexpr static TypeInfo get() noexcept {
      return TypeInfo(std::is_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value,
        std::is_reference<T>::value,
        std::is_pointer<T>::value,
        std::is_void<T>::value,
        (std::is_arithmetic<T>::value || std::is_arithmetic<typename std::remove_reference<T>::type>::value) && !std::is_same<typename std::remove_const<typename std::remove_reference<T>::type>::type, bool>::value,
        std::is_class<T>::value,
        detail::make_type_id<T>(),
        detail::make_type_id<typename detail::Bare_Type<T>::type>(),
        make_name<T>());
    }
  };

}// namespace detail

template<typename T>
constexpr TypeInfo user_type() {
  return detail::GetTypeInfo<T>::get();
}

template<typename T>
TypeInfo user_type(const T &) {
  return detail::GetTypeInfo<T>::get();
}


}// namespace e00::impl::scripting
