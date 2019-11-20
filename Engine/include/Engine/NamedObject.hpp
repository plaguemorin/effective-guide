#pragma once

#include <array>

class NamedObject {
  std::array<char, 10> _name;

protected:
  NamedObject();

  NamedObject(const NamedObject& other);

public:
  void set_name(const char *name);

  [[nodiscard]] const char *name() const { return _name.data(); }
};
