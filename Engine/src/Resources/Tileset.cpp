#include <Engine/Resources/Tileset.hpp>

namespace e00::resource {

Tileset::TileInformation Tileset::_not_found = {};

Tileset::Tileset(std::string name, std::unique_ptr<Bitmap> &&backing, int tile_width, int tile_height)
  : resource::Resource(0, type_id<Tileset>(), std::move(name)),
    _backing_bitmap(std::move(backing)),
    _tile_width(tile_width),
    _tile_height(tile_height),
    _num_tiles_horizontal(_backing_bitmap->size().x() / (_tile_width)) {
}

WindowedBitmap Tileset::tile_bitmap(uint16_t idx) {
  // Bitmap is 64 x 64, tileSize is 16x16
  // _num_tiles_horizontal = 4 (64 / 16 = 4)
  // 0 0------16-----32-----48-----+
  // |
  // |    0       1      2      3
  // |
  //16 0------16-----32-----48-----+
  // |
  // |    4       5      6      7
  // |
  //32 0------16-----32-----48-----+

  const auto x = (idx % _num_tiles_horizontal) * _tile_width;
  const auto y = (idx / _num_tiles_horizontal) * _tile_height;

  return _backing_bitmap->make_window(Vec2<uint16_t>(x, y), Vec2<uint16_t>(_tile_width, _tile_height));
}

}// namespace e00::resource
