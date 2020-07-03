#pragma once

#include <limits>
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

#include <Engine/Resource/Resource.hpp>
#include <Engine/CommonTypes.hpp>

namespace e00::impl {
class Map : public resource::Resource {
  const map_position_t _width;
  const map_position_t _height;

  tile_id_t *_data;

public:
  Map(uint8_t width, uint8_t height);

  Map(Map &&other) noexcept;

  Map(const Map &) = delete;

  ~Map();

  Map &operator=(const Map &rhs) = delete;

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
