#include "Map.hpp"

namespace e00::impl {

Map::Map(uint8_t width, uint8_t height)
  : resource::Resource(0, type_id<Map>(), ""),
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
  _data = other._data;
  other._data = nullptr;
}

Map::~Map() {
  free(_data);
}

}// namespace e00impl
