#pragma once

#include <string>

#include <Engine/ResourceSystem/Info/Type.hpp>

#include "MapPosition.hpp"

namespace e00::resource::info {
struct Portal {
  static constexpr Type type = Type::portal;

  MapPosition from_position;
  MapPosition to_position;

  uint16_t parent_marker_radius;
  bool parent_discoverable;
  std::string from_name_string_id;
};

}
