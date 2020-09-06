#pragma once

#include <string_view>
#include <utility>
#include <Engine/ResourceSystem/Info/Type.hpp>

namespace e00::resource::impl {
inline info::Type ToType(const std::string_view &str) {
  if (str == "Map")
    return info::Type::map;
  if (str == "Actor")
    return info::Type::actor;
  if (str == "Portal")
    return info::Type::portal;
  if (str == "Sprite")
    return info::Type::sprite;
  if (str == "Tileset")
    return info::Type::tileset;

  return info::Type::unknown;
}

inline std::pair<std::string_view, info::Type> ParseIdType(const std::string_view& section) {
  auto colon_pos = section.find_first_of(':');
  if (colon_pos == std::string_view::npos) {
    return std::make_pair(section, info::Type::unknown);
  }

  const auto type = ToType(section.substr(0, colon_pos));
  const auto id = section.substr(colon_pos + 1);

  return std::make_pair(id, type);
}

}
