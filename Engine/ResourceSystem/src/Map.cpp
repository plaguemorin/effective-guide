#include <Engine/ResourceSystem/Resources/Map.hpp>

namespace e00::resource {

Map::Map(std::string name, uint8_t width, uint8_t height)
  : resource::Resource(0, type_id<Map>(), name),
    _width(width),
    _height(height),
    _data(nullptr) {
  _data = static_cast<tile_id_t *>(malloc((width + 1U) * (height + 1U) * sizeof(tile_id_t)));
  //  _data.resize((width + 1U) * (height + 1U));
}

Map::Map(Map &&other) noexcept
  : resource::Resource(std::move(other)),
    _width(other._width),
    _height(other._height),
    _data(nullptr) {
  std::swap(_data, other._data);
}

Map::~Map() {
  free(_data);
}

void Map::set_tile(map_position_t x, map_position_t y, tile_id_t tile_id) {
  if (x >= _width || y >= _height) {
    return;
  }

  _data[static_cast<std::size_t>(y * _width + x)] = tile_id;
}

}// namespace e00impl
