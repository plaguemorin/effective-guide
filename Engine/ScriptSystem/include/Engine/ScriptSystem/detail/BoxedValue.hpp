#pragma once

#include "TypeInfo.hpp"

namespace e00::impl::scripting::detail {
/**
 * Wrapper for holding any support C++ type
 * The script engine only understands BoxedValues for values
 */
struct BoxedValue {
private:
  struct Data {
    virtual ~Data() noexcept = default;
    Data &operator=(const Data &) = delete;
    virtual void *data() noexcept = 0;
    virtual std::unique_ptr<Data> clone() const = 0;
  };

  template<typename T>
  struct DataImpl : Data {
    explicit DataImpl(T data) : _data(std::move(data)) {
    }

    void *data() noexcept override { return &_data; }

    std::unique_ptr<Data> clone() const override {
      return std::make_unique<DataImpl<T>>(_data);
    }

    T _data;
  };

public:
  /// used for explicitly creating a "void" object
  struct VoidType {};

  /// Basic Boxed_Value constructor
  template<typename T, typename = std::enable_if_t<!std::is_same_v<BoxedValue, std::decay_t<T>>>>
  explicit BoxedValue(T &&t, bool t_return_value = false)
    //  : m_data(Object_Data::get(std::forward<T>(t), t_return_value))
    : _info(user_type<T>()),
      _data(new DataImpl<std::decay_t<T>>(std::forward<T>(t))) {
    (void)t;
    (void)t_return_value;
  }

  /// Unknown-type constructor
  BoxedValue() : _info(TypeInfo()), _data(nullptr) {}

  BoxedValue(BoxedValue &&other) = default;

  BoxedValue &operator=(BoxedValue &&rhs) = default;

  BoxedValue(const BoxedValue &other) : _info(other._info), _data(other._data->clone()) {}

  BoxedValue &operator=(const BoxedValue &rhs) {
    if (&rhs != this) {
      _data = rhs._data->clone();
      _info = rhs._info;
    }
    return *this;
  }

  ~BoxedValue() = default;

  const TypeInfo &type() const { return _info; }

  bool is_const() const noexcept { return _info.is_const(); }
  bool is_reference() const noexcept { return _info.is_reference(); }
  bool is_void() const noexcept { return _info.is_void(); }
  bool is_arithmetic() const noexcept { return _info.is_arithmetic(); }
  bool is_undef() const noexcept { return _info.is_undef(); }
  bool is_pointer() const noexcept { return _info.is_pointer(); }

  template<typename ToType>
  ToType &cast() const {
    if (_data && user_type<ToType>() == _info) {
      return *static_cast<ToType *>(_data->data());
    } else {
      throw std::bad_cast();
    }
  }

  template<typename ToType>
  bool valid_cast() const {
    return (_data && user_type<ToType>() == _info);
  }

private:
  TypeInfo _info;
  std::unique_ptr<Data> _data;
};

inline BoxedValue void_var() {
  return BoxedValue(BoxedValue::VoidType());
}


/// The exposed Cast_Helper object that by default just calls the Cast_Helper_Inner
template<typename T>
struct CastHelper {
  static decltype(auto) cast(const BoxedValue &ob) {
    return ob.cast<T>();
  }
};
}// namespace e00::impl::scripting::detail
