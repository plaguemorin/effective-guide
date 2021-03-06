#pragma once

namespace e00 {
using type_t = const void *;

template<typename T>
type_t type_id() {
  static type_t a;
  return &a;
}

class TypedObject {
  type_t _type;

protected:
  explicit TypedObject(type_t type) : _type(type) {}

public:
  [[nodiscard]] const type_t &type() const { return _type; }
};
}// namespace e00impl
