#include <Engine/Resources/Map.hpp>
#include <utility>

namespace e00::resource {

Map::Map(std::string name, uint8_t width, uint8_t height)
  : resource::Resource(0, type_id<Map>(), std::move(name)),
    _size(width, height),
    _layers() {
  for (auto &layer : _layers) {
    layer = nullptr;
  }
}

Map::Layer::Layer(Map::LayerID id) : _id(id), _tiles(nullptr) {
}

Map::Layer::~Layer() {
  delete _tiles;
}

void Map::Layer::set_size(uint16_t size) {
  delete _tiles;
  _tiles = nullptr;

  if (size > 0) {
    _tiles = new uint16_t[size];
  }
}

Map::Map(Map &&other) noexcept
  : resource::Resource(std::move(other)),
    _size(other._size),
    _layers() {
  for (auto &layer : _layers) {
    layer = nullptr;
  }

  std::swap(_layers, other._layers);
}

Map::~Map() {
  for (auto &layer : _layers) {
    delete layer;
  }
}

Map::Layer &Map::add_static_layer(LayerID layer_id) {
  const auto layer_id_num = static_cast<std::underlying_type<LayerID>::type>(layer_id);
  if (!_layers[layer_id_num]) {
    _layers[layer_id_num] = new Layer(layer_id);
    _layers[layer_id_num]->set_size(_size.total());
  }

  return *_layers[layer_id_num];
}

uint16_t Map::tile_at(LayerID layer_id, uint8_t x, uint8_t y) const {
  if (x >= _size.x() || y >= _size.y()) {
    return std::numeric_limits<uint16_t>::max();
  }

  const auto layer_id_num = static_cast<std::underlying_type<LayerID>::type>(layer_id);
  if (!_layers[layer_id_num]) {
    return std::numeric_limits<uint16_t>::max();
  }

  return _layers[layer_id_num]->at(y * _size.y() + x);
}

}// namespace e00::resource
