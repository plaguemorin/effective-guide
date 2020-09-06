#pragma once

#include <string>

#include <Engine/ResourceSystem/Info/Type.hpp>


namespace e00::resource::info {
struct Tileset {
  static constexpr Type type = Type::tileset;

  std::string bitmap;
  uint8_t width;
  uint8_t height;
};
}
