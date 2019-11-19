#pragma once

#include <array>
#include <vector>

#include <Engine/TileSet.hpp>

class Map {
public:
  Map(uint16_t x, uint16_t y, Tileset* tileset);

  ~Map();

  bool is_valid() const;

private:
  struct TileInfo {
    uint16_t tile_id; // ID of this tile in _tileset
  };

  const uint16_t _nb_tiles_x;
  std::vector<TileInfo> _tiles;
  Tileset const *_tileset;

  const TileInfo& info_at_screen_space(uint32_t x, uint32_t y) const {
    return info_at_tile(x / _tileset->tile_width_px(), y / _tileset->tile_height_px());
  }

  TileInfo& info_at_screen_space(uint32_t x, uint32_t y) {
    return info_at_tile(x / _tileset->tile_width_px(), y / _tileset->tile_height_px());
  }

  const TileInfo& info_at_tile(uint16_t x, uint16_t y) const { return _tiles.at(y * _nb_tiles_x + x); }

  TileInfo& info_at_tile(uint16_t x, uint16_t y) { return _tiles.at(y * _nb_tiles_x + x); }
};
