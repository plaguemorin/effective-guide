#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace e00 {
class NamedObject {
  std::string _name;

protected:
  NamedObject() : _name{ "UNNAMED" } {}

  explicit NamedObject(std::string_view name) : _name(name) {}

  NamedObject(NamedObject &&swap) noexcept : _name(std::move(swap._name)) {}

  ~NamedObject() = default;

public:
  [[nodiscard]] std::string_view name() const { return _name; }

  NamedObject(const NamedObject &other) = delete;

  NamedObject &operator=(const NamedObject &rhs) = delete;

  NamedObject &operator=(NamedObject &&rhs) noexcept {
    if (&rhs != this) {
      std::swap(_name, rhs._name);
    }
    return *this;
  }
};
}// namespace e00impl
