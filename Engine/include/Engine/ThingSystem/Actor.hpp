#pragma once

#include <Engine/ThingSystem/Thing.hpp>

namespace thing {
class Actor : public Thing {
  uint16_t _health;

protected:
  explicit Actor(type_t type);

public:
  Actor();

  ~Actor() override;

  Actor(const Actor&) = delete;
};
}
