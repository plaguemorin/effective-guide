#pragma once

#include <cstdint>

namespace e00 {
using type_t = const void *;

using resource_file_type_t = uint32_t;
using resource_id_t = uint32_t;

using tile_id_t = std::uint8_t;
using map_position_t = std::uint8_t;
using map_position_linear_t = std::uint16_t;
}// namespace e00impl
