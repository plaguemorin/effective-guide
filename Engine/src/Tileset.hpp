#pragma once

#include <cstdint>

#include <Engine/ResourceSystem/Resources/Bitmap.hpp>

namespace e00::impl {
class Tileset {
public:
  uint8_t width() const;
  uint8_t height() const;

  resource::Bitmap *tile(uint16_t idx);
};
}
