#include "Engine/TileSet.hpp"

Tileset::Tileset(uint16_t nb_tiles, uint16_t tile_width_px, uint16_t tile_height_px)
        : _tile_width_px(tile_width_px), _tile_height_px(tile_height_px) {
  _infos.resize(nb_tiles);
}

Tileset::~Tileset() = default;
