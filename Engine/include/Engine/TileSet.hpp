#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <Engine/Resource/Sprite.hpp>

class Tileset {
public:
  struct Tile {
    resource::resource_id_t texture_id;
    uint16_t collision_bitmap; // 4 x 4 grid of collision
    bool opaque; // does this tile block light
  };

  Tileset(uint16_t nb_tiles, uint16_t tile_width_px, uint16_t tile_height_px);

  ~Tileset();

  uint16_t tile_width_px() const { return _tile_width_px; }

  uint16_t tile_height_px() const { return _tile_height_px; }

  uint16_t max_tile_id() const { return _infos.size(); }

  const Tile& at(uint16_t tile_id) const { return _infos.at(tile_id); }

  Tile& at(uint16_t tile_id) { return _infos.at(tile_id); }

  const Tile& operator[](uint16_t i) const { return at(i); }

  Tile& operator[](uint16_t i) { return at(i); }

  void add(uint16_t id, const std::string_view& path);

  bool has_collision(uint16_t tile_id, uint16_t x, uint16_t y) const {
    const auto& tile = _infos.at(tile_id);
    // If the tile has no collision at all, then no collision is possible
    if (tile.collision_bitmap == 0)
      return false;

    // If the tile is fully blocking, then we are colliding no matter where
    if (tile.collision_bitmap == std::numeric_limits<decltype(Tile::collision_bitmap)>::max())
      return true;

    const auto bucket_width = tile_width_px() / 4;
    const auto bucket_height = tile_height_px() / 4;

    const auto x_bucket = x / bucket_width;
    const auto y_bucket = y / bucket_height;

    const auto bucket = 1u << (y_bucket * 4 + x_bucket);

    return (tile.collision_bitmap & bucket) > 0;
  }

private:
  uint16_t _tile_width_px;
  uint16_t _tile_height_px;
  std::vector<Tile> _infos;
};
