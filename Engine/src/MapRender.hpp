#pragma once

#include <Engine/Resources/ResourcePtr.hpp>
#include <Engine/Resources/Map.hpp>
#include <Engine/Resources/Tileset.hpp>
#include <Engine/System/OutputScreen.hpp>
#include <utility>

namespace e00 {
class MapRender {
  resource::ResourcePtr<resource::Tileset> _tileset;
  resource::ResourcePtr<resource::Map> _map;

public:
  MapRender(resource::ResourcePtr<resource::Tileset> tileset, resource::ResourcePtr<resource::Map> map) : _tileset(std::move(tileset)), _map(std::move(map)) {}

  ~MapRender() = default;

  void render(sys::OutputScreen* output);
};
}// namespace e00
