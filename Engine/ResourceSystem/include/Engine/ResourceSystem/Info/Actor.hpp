#pragma once

#include <string>

#include <Engine/ResourceSystem/Info/Type.hpp>

#include "MapPosition.hpp"

namespace e00::resource::info {
struct Actor {
  static constexpr Type type = Type::actor;

  enum class EssentialStatus {
    none,
    player_killable,// "Protected"
    essential,
  };

  enum class RelationshipToPlayer {
    neutral,
    ally,
    enemy,
  };

  enum class Assistance {
    none,
    player,
    player_and_ally,
  };

  std::string sprite;
  MapPosition position;
  uint16_t angle;
  bool deaf;
  EssentialStatus essential_status;
  RelationshipToPlayer relationship_to_player;
  uint16_t aggression_level;
  Assistance assistance;
  std::string ai;
};
}
