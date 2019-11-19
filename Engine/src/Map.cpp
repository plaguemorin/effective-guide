#include "Engine/Map.hpp"

Map::Map(uint16_t x, uint16_t y, Tileset *tileset)
        : _nb_tiles_x(x), _nb_tiles_y(y), _tileset(tileset) {
  _tiles.resize(x * y);
}

Map::~Map() {

}

bool Map::is_valid() const {
  for (const auto& i : _tiles) {
    if (i.tile_id > _tileset->max_tile_id())
      return false;
  }

  return true;
}
