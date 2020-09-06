#pragma once

#include <cstdint>
#include <Engine/ResourceSystem/Info/Type.hpp>

namespace e00::resource::info {
struct Sprite {
  static constexpr Type type = Type::sprite;

  uint8_t width;
  uint8_t height;
};
}
