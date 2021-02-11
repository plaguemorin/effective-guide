#include "MapRender.hpp"

namespace e00 {

void MapRender::render(sys::OutputScreen *output) {
  const auto output_size = output->size();
  const auto tileset_size = _tileset->tileSize();
  const auto map_size = _map->size();

  // The tiles might have to be shifted if we're not at a perfect multiplier for the tile size
  uint16_t x_offs = 0;
  uint16_t y_offs = 0;

  // Top left tile (in map position) is offset by...
  uint8_t x_start = 0;
  uint8_t y_start = 0;

  // How many tiles can we fit ?
  const auto tiles_per_screen = output_size / tileset_size;

  // Do we fit ?


  for (uint16_t y = 0; y < tiles_per_screen.y(); ++y) {
    for (uint16_t x = 0; x < tiles_per_screen.x(); ++x) {
      const uint8_t tile_id = _map->tile_at(resource::Map::LayerID::BACKGROUND, x + x_start, y + y_start);

      output->blit_bitmap(
        Vec2<uint16_t>(x * tileset_size.x() - x_offs, y * tileset_size.y() - y_offs),
        _tileset->tile_bitmap(tile_id));
    }
  }
}

}// namespace e00
