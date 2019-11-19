#pragma once

#include <array>

class NamedObject {
  std::array<char, 10> _name;
public:
  NamedObject();

  NamedObject(const NamedObject& other);

  virtual ~NamedObject();

  void set_name(const char *name);

  [[nodiscard]] const char *name() const { return _name.data(); }
};
