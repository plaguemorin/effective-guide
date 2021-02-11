#pragma once

#include <limits>
#include <cstdint>
#include <type_traits>
#include <array>

#include <Engine/Math.hpp>
#include <Engine/Resource.hpp>

namespace e00::resource {
using tile_id_t = std::uint8_t;
using map_position_t = std::uint8_t;

class Map : public Resource {
  const Vec2<map_position_t> _size;
  std::string _tileset_name;

public:
  enum class LayerID : uint8_t {
    BACKGROUND = 0,
    CRITTERS,
    PLAYER,
    SKY
  };

  class Layer {
    Map::LayerID _id;
    uint16_t *_tiles;

    template<std::size_t I>
    void set_data() {}

    template<std::size_t I, typename N, typename... Args>
    void set_data(N item, Args... arg) {
      _tiles[I] = static_cast<uint16_t>(item);
      set_data<I + 1>(arg...);
    }

  public:
    explicit Layer(LayerID id);

    ~Layer();

    void set_size(uint16_t size);

    [[nodiscard]] LayerID id() const { return _id; }

    [[nodiscard]] uint16_t at(uint8_t position) const { return _tiles[position]; }

    template<typename... Args>
    void set_data(Args... arg) {
      set_data<0>(arg...);
    }
  };

private:
  std::array<Layer *, 4> _layers;

public:
  Map(std::string name, uint8_t width, uint8_t height);

  Map(Map &&other) noexcept;

  Map(const Map &) = delete;

  ~Map() override;

  Map &operator=(const Map &rhs) = delete;

  void set_tileset_name(std::string tileset_name) { _tileset_name = std::move(tileset_name); }

  [[nodiscard]] const std::string &tileset_name() const { return _tileset_name; }

  [[nodiscard]] decltype(_size) size() const { return _size; }

  Layer &add_static_layer(LayerID layer_id);

  uint16_t tile_at(LayerID layer_id, uint8_t x, uint8_t y) const;
};
}// namespace e00::resource
