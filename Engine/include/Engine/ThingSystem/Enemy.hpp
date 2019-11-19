#pragma once

#include <Engine/ThingSystem/Actor.hpp>

namespace thing {
class Enemy : public Actor {
public:
  Enemy();

  ~Enemy() override;
};
}
