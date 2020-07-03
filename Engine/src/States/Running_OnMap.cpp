#include "Running_OnMap.hpp"

#include <Engine/System/OutputScreen.hpp>
#include <Engine.hpp>

namespace e00::impl {
Running_OnMap::Running_OnMap(Map &&map) : _map(std::move(map)) {
}

Running_OnMap::~Running_OnMap() {
}

bool Running_OnMap::needs_steady_updates() const {
  return true;
}

State *Running_OnMap::update(const std::chrono::milliseconds &delta_since_last_update) {
  (void)delta_since_last_update;

  return nullptr;
}

void Running_OnMap::render() {
  auto *output_screen = _engine->get_output_screen();
  if (output_screen == nullptr) {
    return;
  }

  map_position_t tile_start_x = _camera_pos.x / _tileset.width();
  map_position_t tile_start_y = _camera_pos.y / _tileset.height();

  uint8_t draw_offset_x = _camera_pos.x % _tileset.width();
  uint8_t draw_offset_y = _camera_pos.y % _tileset.height();

  for (map_position_t y = 0; y < _screen_y_tiles; ++y) {
    for (map_position_t x = 0; x < _screen_x_tiles; ++x) {
      const auto index = _map.tile_index_at(x + tile_start_x, y + tile_start_y);

      if (auto *source = _tileset.tile(index)) {
        output_screen->blit_bitmap(
          Vec2I(x * _tileset.width() - draw_offset_x, y * _tileset.height() - draw_offset_y),
          source);
      }
    }
  }
}
}// namespace e00impl
