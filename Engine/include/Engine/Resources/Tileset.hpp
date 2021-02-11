#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include <Engine/Resource.hpp>
#include <Engine/Resources/Bitmap.hpp>

namespace e00::resource {
class Tileset : public Resource {
public:
  struct TileInformation {

  };

private:
  static TileInformation _not_found;

  std::unique_ptr<Bitmap> _backing_bitmap;

  const uint16_t _tile_width;
  const uint16_t _tile_height;

  // Computed values
  const uint16_t _num_tiles_horizontal;

public:
  Tileset(std::string name, std::unique_ptr<Bitmap> &&backing, int tile_width, int tile_height);

  ~Tileset() override = default;

  [[nodiscard]] Vec2<std::remove_cv<decltype(_tile_width)>::type> tileSize() const { return {_tile_width, _tile_height}; }

  [[nodiscard]] TileInformation& information(uint16_t idx) const { return _not_found; }

  WindowedBitmap tile_bitmap(uint16_t idx);
};
}// namespace e00::resource
