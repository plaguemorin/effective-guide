#include "Engine/ThingSystem/Manager.hpp"

namespace e00::thing {

Thing *Manager::at(uint16_t tile_x, uint16_t tile_y) const {
  for (const auto& thing : container()) {
    if (thing
        && thing->position().x == tile_x
        && thing->position().y == tile_y) {
      return thing;
    }
  }
  return nullptr;
}
}
