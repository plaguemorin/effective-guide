#pragma once

#include <string>
#include <cstdint>

#include <Engine/ResourceSystem/Info/Type.hpp>

namespace e00::resource::info {
struct Map {
  static constexpr Type type = Type::map;

  enum class DataType {
    unknown,
    text
  };

  std::string tileset;
  std::string data;
  DataType data_type;
  uint8_t width;
  uint8_t height;
};
}
