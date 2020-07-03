#pragma once

#include <type_traits>
#include <Engine/Resource/Resource.hpp>

namespace e00 {
template<typename T>
class SharedResource final {
  static_assert(std::is_convertible<T *, resource::Resource *>::value, "Class must be subclass of Resource");
  T *_resource;

public:
  SharedResource(std::nullptr_t nullopt) : _resource(nullptr) {}

  SharedResource();

  ~SharedResource() {}

  [[nodiscard]] T const *operator*() const { return _resource; }
  [[nodiscard]] T *operator*() { return _resource; }
};
}// namespace e00
