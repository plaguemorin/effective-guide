#pragma once

#include <string>
#include <cstdint>

namespace e00::resource::info {
struct MapPosition {
  std::string map_name;
  uint16_t position_x;
  uint16_t position_y;
};
}
