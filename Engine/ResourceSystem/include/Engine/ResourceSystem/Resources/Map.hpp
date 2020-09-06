#pragma once

#include <limits>
#include <cstdint>
#include <type_traits>

#include <Engine/ResourceSystem/Resource.hpp>

namespace e00::resource {
using tile_id_t = std::uint8_t;
using map_position_t = std::uint8_t;

class Map : public Resource {
  const map_position_t _width;
  const map_position_t _height;
  std::string _tileset_name;

  tile_id_t *_data;

public:
  Map(std::string name, uint8_t width, uint8_t height);

  Map(Map &&other) noexcept;

  Map(const Map &) = delete;

  ~Map() override;

  Map &operator=(const Map &rhs) = delete;

  void set_tile(map_position_t x, map_position_t y, tile_id_t tile_id);

  void set_tileset_name(std::string tileset_name) { _tileset_name = std::move(tileset_name); }

  [[nodiscard]] const std::string& tileset_name() const { return _tileset_name; }

  [[nodiscard]] map_position_t width() const { return _width; }

  [[nodiscard]] map_position_t height() const { return _height; }

  template<typename T>
  typename std::enable_if<std::is_unsigned<T>::value, tile_id_t>::type tile_index_at(T x, T y) const {
    if (x >= _width || y >= _height) {
      return std::numeric_limits<tile_id_t>::max();
    }

    return _data[static_cast<std::size_t>(y * _width + x)];
  }

  template<typename T>
  typename std::enable_if<std::is_signed<T>::value, tile_id_t>::type tile_index_at(T x, T y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      return std::numeric_limits<tile_id_t>::max();
    }

    return _data[static_cast<std::size_t>(y * _width + x)];
  }

};
}// namespace e00impl
