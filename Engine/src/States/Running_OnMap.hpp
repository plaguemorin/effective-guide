#pragma once

#include "State.hpp"
#include <Engine/Math.hpp>

#include <Engine/ResourceSystem/Resources/Map.hpp>
#include "Tileset.hpp"

namespace e00::impl {
class Running_OnMap : public State {
  Tileset _tileset;
  Vec2UI _camera_pos;

  uint8_t _screen_y_tiles;
  uint8_t _screen_x_tiles;

public:
  Running_OnMap();
  ~Running_OnMap() override;

  bool needs_steady_updates() const override;
  State *update(const std::chrono::milliseconds &delta_since_last_update) override;
  void render() override;
};
}// namespace e00impl
