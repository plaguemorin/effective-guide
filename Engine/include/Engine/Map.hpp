#pragma once

#include <array>
#include <vector>

#include <Engine/TileSet.hpp>

class Map {
public:
  struct TileInfo {
    uint16_t tile_id; // ID of this tile in _tileset
  };

  // for debugging...
  template<typename ...Args>
  static Map create_from_data(Tileset *tileset, int nb_tiles_x, Args&& ...args) {
    Map aMap(nb_tiles_x, sizeof...(args) / nb_tiles_x, tileset);
    aMap.set_static_map_data<0>(std::forward<Args>(args)...);
    return aMap;
  }

  Map(uint16_t x, uint16_t y, Tileset *tileset);

  ~Map();

  [[nodiscard]] bool is_valid() const;

  [[nodiscard]] uint16_t height() const { return _nb_tiles_y; }

  [[nodiscard]] uint16_t width() const { return _nb_tiles_x; }


  const TileInfo& info_at_screen_space(uint32_t x, uint32_t y) const {
    return info_at_tile(x / _tileset->tile_width_px(), y / _tileset->tile_height_px());
  }

  TileInfo& info_at_screen_space(uint32_t x, uint32_t y) {
    return info_at_tile(x / _tileset->tile_width_px(), y / _tileset->tile_height_px());
  }

  const TileInfo& info_at_tile(uint16_t x, uint16_t y) const { return _tiles.at(y * _nb_tiles_x + x); }

  TileInfo& info_at_tile(uint16_t x, uint16_t y) { return _tiles.at(y * _nb_tiles_x + x); }

private:
  const uint16_t _nb_tiles_x;
  const uint16_t _nb_tiles_y;
  std::vector<TileInfo> _tiles;
  Tileset const *_tileset;

  template<int position, typename ...Args>
  void set_static_map_data(uint16_t tile_id, Args&& ...args) {
    _tiles[position].tile_id = tile_id;
    set_static_map_data<position + 1>(std::forward<Args>(args)...);
  }

  template<int position>
  void set_static_map_data(uint16_t tile_id) {
    _tiles[position].tile_id = tile_id;
  }


};
