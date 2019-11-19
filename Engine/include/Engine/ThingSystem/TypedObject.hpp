#pragma once

namespace thing {
using thing_type_t = const void *;

template<typename T>
thing_type_t type_id() {
  static thing_type_t a;
  return &a;
}


class TypedObject {
  thing_type_t _type;

protected:
  explicit TypedObject(thing_type_t type) : _type(type) {}

public:
  TypedObject() : _type{} {}

  virtual ~TypedObject() = default;

  [[nodiscard]] const thing_type_t& type() const { return _type; }
};
}
