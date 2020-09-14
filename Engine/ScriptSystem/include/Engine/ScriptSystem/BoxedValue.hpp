#pragma once

#include <memory>

#include "TypeInfo.hpp"

namespace e00::impl::scripting {
/**
 * Wrapper for holding any support C++ type
 * The script engine only understands BoxedValues for values
 */
struct BoxedValue {
public:
  /// used for explicitly creating a "void" object
  struct VoidType {};

private:
  struct Data {
    Data() = default;
    Data &operator=(const Data &) = delete;
    virtual ~Data() noexcept = default;
    virtual void *data() noexcept = 0;
    virtual std::unique_ptr<Data> clone() const = 0;
  };

  struct DataCtrl {
    int refs;

    DataCtrl() : refs(0) {}
  };

  template<typename T>
  struct DataImpl : Data {
    DataCtrl *_ctrl;
    T _data;

    explicit DataImpl(T mdata)
      : _data(std::move(mdata)),
        _ctrl(new DataCtrl()) {
      _ctrl->refs++;
    }

    DataImpl(T mdata, DataCtrl *mctrl)
      : _ctrl(mctrl),
        _data(mdata) {
      _ctrl->refs++;
    }

    ~DataImpl() override {
      _ctrl->refs--;
      if (_ctrl->refs <= 0) {
        delete _ctrl;
        delete _data;
      }
    }

    void *data() noexcept override { return _data; }

    std::unique_ptr<Data> clone() const override {
      return std::make_unique<DataImpl<T>>(_data, _ctrl);
    }

    DataImpl &operator=(const DataImpl &) = delete;
  };


  struct DataBuilder {
    template<typename T>
    constexpr static bool is_const() {
      return std::is_const<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value;
    }

    //
    //    static std::unique_ptr<Data> get(BoxedValue::VoidType) {}
    //
    //    template<typename T>
    //    static std::unique_ptr<Data> get(std::unique_ptr<T> &&obj) {}
    //
    //    template<typename T>
    //    static std::unique_ptr<Data> get(std::reference_wrapper<T> obj) {
    //      auto p = &obj.get();
    //      return std::make_unique<DataImpl<std::decay_t<decltype(p)>>>(std::forward<decltype(p)>(p));
    //    }
    //

    template<typename T>
    static std::unique_ptr<Data> get(T *t) {
      return std::make_unique<DataImpl<std::decay_t<decltype(t)>>>(std::forward<decltype(t)>(t));
    }

    template<typename T>
    static std::unique_ptr<Data> get(const T *t) {
      return std::make_unique<DataImpl<std::decay_t<decltype(t)>>>(std::forward<decltype(t)>(t));
    }

    template<typename T>
    static std::unique_ptr<Data> get(T t) {
      auto p = new T(std::move(t));
      return get(p);
    }
  };

public:
  /// Basic Boxed_Value constructor
  template<typename T, typename = std::enable_if_t<!std::is_same_v<BoxedValue, std::decay_t<T>>>>
  explicit BoxedValue(T &&t, bool t_return_value = false)
    : _info(user_type<T>()),
      _is_return(t_return_value),
      _data(DataBuilder::get(std::forward<T>(t))) {
  }

  // Report as another type (for pointers)
  template<typename T, typename = std::enable_if_t<!std::is_same_v<BoxedValue, std::decay_t<T>>>>
  explicit BoxedValue(T &&t, TypeInfo type, bool t_return_value = false)
    : _info(type),
      _is_return(t_return_value),
      _data(DataBuilder::get(std::forward<T>(t))) {
  }

  // Unknown-type constructor
  BoxedValue() : _info(TypeInfo()), _is_return(false), _data(nullptr) {}

  BoxedValue(BoxedValue &&other) = default;

  BoxedValue &operator=(BoxedValue &&rhs) = default;

  BoxedValue(const BoxedValue &other)
    : _info(other._info),
      _is_return(other._is_return),
      _data(other._data->clone()) {}

  BoxedValue &operator=(const BoxedValue &rhs) {
    if (&rhs != this) {
      _data = rhs._data->clone();
      _info = rhs._info;
      _is_return = rhs._is_return;
    }
    return *this;
  }

  ~BoxedValue() = default;

  const TypeInfo &get_type_info() const { return _info; }

  bool is_const() const noexcept { return _info.is_const(); }
  bool is_reference() const noexcept { return _info.is_reference(); }
  bool is_void() const noexcept { return _info.is_void(); }
  bool is_arithmetic() const noexcept { return _info.is_arithmetic(); }
  bool is_undef() const noexcept { return _info.is_undef(); }
  bool is_pointer() const noexcept { return _info.is_pointer(); }

  const void *get_const_ptr() const noexcept {
    return _data->data();
  }

  void *get_ptr() const noexcept {
    return _data->data();
  }

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

  template<typename VisitorType, typename Callable>
  bool matches(Callable &&vis) const {
    if (user_type<VisitorType>() == _info) {
      vis(cast<VisitorType>());
      return true;
    }
    return false;
  }

private:
  TypeInfo _info;
  bool _is_return;
  std::unique_ptr<Data> _data;
};

inline BoxedValue void_var() {
  return BoxedValue(BoxedValue::VoidType());
}
}// namespace e00::impl::scripting
