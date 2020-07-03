#pragma once

#include <vector>
#include <Engine/TypedObject.hpp>

namespace e00::impl {
template<typename ObjectContained>
class TypedObjectContainer {
  static_assert(std::is_convertible<ObjectContained *, TypedObject *>::value, "Class must be subclass of TypedObject");

  std::vector<ObjectContained *> _objects;

  template<typename T>
  void validate() const {
    static_assert(std::is_convertible<T *, ObjectContained *>::value, "Class must be subclass of ObjectContained");
  }

protected:
  const auto &container() const { return _objects; }

  auto &container() { return _objects; }

public:
  bool register_object(ObjectContained *obj) {
    for (auto &ptr : _objects) {
      if (!ptr) {
        ptr = obj;
        return true;
      }
    }

    return false;
  }

  bool unreigster_object(const ObjectContained *obj) {
    for (auto &ptr : _objects) {
      if (ptr == obj) {
        ptr = nullptr;
        return true;
      }
    }
    return false;
  }

  template<typename T>
  void unregister_all() {
    validate<T>();
    for (auto &ptr : _objects) {
      if (ptr->type == type_id<T>())
        ptr = nullptr;
    }
  }

  template<typename Fnc, typename... Args>
  void all(Fnc func, Args &&... args) const {
    for (const auto &obj : _objects) {
      func(obj, std::forward<Args>(args)...);
    }
  }

  template<typename Fnc, typename... Args>
  void all(Fnc func, Args &&... args) {
    for (auto &obj : _objects) {
      func(obj, std::forward<Args>(args)...);
    }
  }

  template<typename T, typename Fnc, typename... Args>
  void all_of_type(Fnc func, Args &&... args) const {
    validate<T>();
    for (const auto &obj : _objects) {
      if (obj->type() == type_id<T>()) {
        func(static_cast<T *>(obj), std::forward<Args>(args)...);
      }
    }
  }

  template<typename T, typename Fnc, typename... Args>
  void all_of_type(Fnc func, Args &&... args) {
    validate<T>();
    for (auto &obj : _objects) {
      if (obj && obj->type() == type_id<T>()) {
        func(static_cast<T *>(obj), std::forward<Args>(args)...);
      }
    }
  }
};
}// namespace e00impl
