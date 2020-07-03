#include "Tileset.hpp"


namespace e00::impl {

uint8_t Tileset::width() const {
  return 32;
}
uint8_t Tileset::height() const {
  return 32;
}
resource::Bitmap *Tileset::tile(uint16_t idx) {
  (void)idx;
  return nullptr;
}
}// namespace e00::impl
