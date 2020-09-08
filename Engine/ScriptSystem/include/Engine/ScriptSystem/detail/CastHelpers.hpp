#pragma once

#include <stdexcept>

#include "../BoxedValue.hpp"

namespace e00::impl::scripting::detail {
// Cast_Helper_Inner helper classes

template<typename T>
constexpr T *throw_if_null(T *t) {
  if (t) {
    return t;
  }
  throw std::runtime_error("Attempted to dereference null BoxedValue");
}

template<typename T>
static const T *verify_type_no_throw(const BoxedValue &ob, const TypeInfo &ti, const T *ptr) {
  if (ob.get_type_info() == ti) {
    return ptr;
  } else {
    throw std::bad_cast();
  }
}

template<typename T>
static T *verify_type_no_throw(const BoxedValue &ob, const TypeInfo &ti, T *ptr) {
  if (!ob.is_const() && ob.get_type_info() == ti) {
    return ptr;
  } else {
    throw std::bad_cast();
  }
}


template<typename T>
static const T *verify_type(const BoxedValue &ob, const TypeInfo &ti, const T *ptr) {
  if (ob.get_type_info().bare_equal_type_info(ti)) {
    return throw_if_null(ptr);
  } else {
    throw std::bad_cast();
  }
}

template<typename T>
static T *verify_type(const BoxedValue &ob, const TypeInfo &ti, T *ptr) {
  if (!ob.is_const() && ob.get_type_info().bare_equal_type_info(ti)) {
    return throw_if_null(ptr);
  } else {
    throw std::bad_cast();
  }
}

/// Generic Cast_Helper_Inner, for casting to any type
template<typename Result>
struct Cast_Helper_Inner {
  static Result cast(const BoxedValue &ob) {
    return *static_cast<const Result *>(verify_type(ob, user_type<Result>(), ob.get_const_ptr()));
  }
};

template<typename Result>
struct Cast_Helper_Inner<const Result> : Cast_Helper_Inner<Result> {
};


/// Cast_Helper_Inner for casting to a const * type
template<typename Result>
struct Cast_Helper_Inner<const Result *> {
  static const Result *cast(const BoxedValue &ob) {
    return static_cast<const Result *>(verify_type_no_throw(ob, user_type<Result>(), ob.get_const_ptr()));
  }
};

/// Cast_Helper_Inner for casting to a * type
template<typename Result>
struct Cast_Helper_Inner<Result *> {
  static Result *cast(const BoxedValue &ob) {
    return static_cast<Result *>(verify_type_no_throw(ob, user_type<Result>(), ob.get_ptr()));
  }
};

template<typename Result>
struct Cast_Helper_Inner<Result *const &> : public Cast_Helper_Inner<Result *> {
};

template<typename Result>
struct Cast_Helper_Inner<const Result *const &> : public Cast_Helper_Inner<const Result *> {
};


/// Cast_Helper_Inner for casting to a & type
template<typename Result>
struct Cast_Helper_Inner<const Result &> {
  static const Result &cast(const BoxedValue &ob) {
    return *static_cast<const Result *>(verify_type(ob, user_type<Result>(), ob.get_const_ptr()));
  }
};


/// Cast_Helper_Inner for casting to a & type
template<typename Result>
struct Cast_Helper_Inner<Result &> {
  static Result &cast(const BoxedValue &ob) {
    return *static_cast<Result *>(verify_type(ob, user_type<Result>(), ob.get_ptr()));
  }
};

/// Cast_Helper_Inner for casting to a && type
template<typename Result>
struct Cast_Helper_Inner<Result &&> {
  static Result &&cast(const BoxedValue &ob) {
    return std::move(*static_cast<Result *>(verify_type(ob, user_type<Result>(), ob.get_ptr())));
  }
};

/// Cast_Helper_Inner for casting to a std::unique_ptr<> && type
/// \todo Fix the fact that this has to be in a shared_ptr for now
template<typename Result>
struct Cast_Helper_Inner<std::unique_ptr<Result> &&> {
  static std::unique_ptr<Result> &&cast(const BoxedValue &ob) {
    return std::move(*(ob.get().cast<std::shared_ptr<std::unique_ptr<Result>>>()));
  }
};

/// Cast_Helper_Inner for casting to a std::unique_ptr<> & type
/// \todo Fix the fact that this has to be in a shared_ptr for now
template<typename Result>
struct Cast_Helper_Inner<std::unique_ptr<Result> &> {
  static std::unique_ptr<Result> &cast(const BoxedValue &ob) {
    return *(ob.get().cast<std::shared_ptr<std::unique_ptr<Result>>>());
  }
};

/// Cast_Helper_Inner for casting to a std::unique_ptr<> & type
/// \todo Fix the fact that this has to be in a shared_ptr for now
template<typename Result>
struct Cast_Helper_Inner<const std::unique_ptr<Result> &> {
  static std::unique_ptr<Result> &cast(const BoxedValue &ob) {
    return *(ob.get().cast<std::shared_ptr<std::unique_ptr<Result>>>());
  }
};


/// Cast_Helper_Inner for casting to a std::shared_ptr<> type
template<typename Result>
struct Cast_Helper_Inner<std::shared_ptr<Result>> {
  static auto cast(const BoxedValue &ob) {
    return ob.get().cast<std::shared_ptr<Result>>();
  }
};

/// Cast_Helper_Inner for casting to a std::shared_ptr<const> type
template<typename Result>
struct Cast_Helper_Inner<std::shared_ptr<const Result>> {
  static auto cast(const BoxedValue &ob) {
    if (!ob.is_const()) {
      return std::const_pointer_cast<const Result>(ob.get().cast<std::shared_ptr<Result>>());
    } else {
      return ob.get().cast<std::shared_ptr<const Result>>();
    }
  }
};

/// Cast_Helper_Inner for casting to a const std::shared_ptr<> & type
template<typename Result>
struct Cast_Helper_Inner<const std::shared_ptr<Result>> : Cast_Helper_Inner<std::shared_ptr<Result>> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::shared_ptr<Result> &> : Cast_Helper_Inner<std::shared_ptr<Result>> {
};

template<typename Result>
struct Cast_Helper_Inner<std::shared_ptr<Result> &> {
  static_assert(!std::is_const<Result>::value, "Non-const reference to std::shared_ptr<const T> is not supported");
  static auto cast(const BoxedValue &ob) {
    std::shared_ptr<Result> &res = ob.get().cast<std::shared_ptr<Result>>();
    return ob.pointer_sentinel(res);
  }
};


/// Cast_Helper_Inner for casting to a const std::shared_ptr<const> & type
template<typename Result>
struct Cast_Helper_Inner<const std::shared_ptr<const Result>> : Cast_Helper_Inner<std::shared_ptr<const Result>> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::shared_ptr<const Result> &> : Cast_Helper_Inner<std::shared_ptr<const Result>> {
};


/// Cast_Helper_Inner for casting to a BoxedValue type
template<>
struct Cast_Helper_Inner<BoxedValue> {
  static BoxedValue cast(const BoxedValue &ob) {
    return ob;
  }
};

/// Cast_Helper_Inner for casting to a BoxedValue & type
template<>
struct Cast_Helper_Inner<BoxedValue &> {
  static std::reference_wrapper<BoxedValue> cast(const BoxedValue &ob) {
    return std::ref(const_cast<BoxedValue &>(ob));
  }
};


/// Cast_Helper_Inner for casting to a const BoxedValue & type
template<>
struct Cast_Helper_Inner<const BoxedValue> : Cast_Helper_Inner<BoxedValue> {
};

template<>
struct Cast_Helper_Inner<const BoxedValue &> : Cast_Helper_Inner<BoxedValue> {
};


/// Cast_Helper_Inner for casting to a std::reference_wrapper type
template<typename Result>
struct Cast_Helper_Inner<std::reference_wrapper<Result>> : Cast_Helper_Inner<Result &> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::reference_wrapper<Result>> : Cast_Helper_Inner<Result &> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::reference_wrapper<Result> &> : Cast_Helper_Inner<Result &> {
};

template<typename Result>
struct Cast_Helper_Inner<std::reference_wrapper<const Result>> : Cast_Helper_Inner<const Result &> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::reference_wrapper<const Result>> : Cast_Helper_Inner<const Result &> {
};

template<typename Result>
struct Cast_Helper_Inner<const std::reference_wrapper<const Result> &> : Cast_Helper_Inner<const Result &> {
};

/// The exposed Cast_Helper object that by default just calls the Cast_Helper_Inner
template<typename T>
struct CastHelper {
  static decltype(auto) cast(const BoxedValue &ob) {
    return (Cast_Helper_Inner<T>::cast(ob));
  }
};
}// namespace e00::impl::scripting::detail
