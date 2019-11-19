#pragma once

#include <cstdint>
#include <vector>

class Tileset {
public:
  struct Tile {
    uint16_t texture_id;
    uint32_t collision_bitmap;
  };

  Tileset(uint16_t nb_tiles, uint16_t tile_width_px, uint16_t tile_height_px);

  virtual ~Tileset();

  uint16_t tile_width_px() const { return _tile_width_px; }

  uint16_t tile_height_px() const { return _tile_height_px; }

  uint16_t max_tile_id() const { return _infos.size(); }

  const Tile& at(uint16_t tile_id) const { return _infos.at(tile_id); }

  Tile& at(uint16_t tile_id) { return _infos.at(tile_id); }

  const Tile& operator[](uint16_t i) const { return at(i); }

  Tile& operator[](uint16_t i) { return at(i); }

private:
  uint16_t _tile_width_px;
  uint16_t _tile_height_px;
  std::vector<Tile> _infos;
};
