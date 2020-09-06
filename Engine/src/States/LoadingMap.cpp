#include "LoadingMap.hpp"

#include "Engine.hpp"
#include "Running_OnMap.hpp"

namespace e00::impl {
Loading_Map::Loading_Map(std::string map_name)
  : _logger("Loading_Map"),
    _map_path(std::move(map_name)),
    _stage(LOAD_MAP) {
}

State *Loading_Map::update(const std::chrono::milliseconds &delta_since_last_update) {
  (void)delta_since_last_update;

  switch (_stage) {
    case LOAD_MAP: load_map(); return nullptr;
    case LOAD_TILESET: load_tileset(); return nullptr;
    case COMPUTE: break;
    case ERROR: break;
  }

  //  return new Running_OnMap(_map_parser.build());
  return nullptr;
}

void Loading_Map::render() {
}

void Loading_Map::load_map() {
  // Load data of map_name
  _map = resource_manager()->load_resource<resource::Map>(_map_path);

  if (_map) {
    _stage = LOAD_TILESET;
  } else {
    _stage = ERROR;
  }
}

void Loading_Map::load_tileset() {
  _logger.info(SourceLocation(), "Loading tileset");
  resource_manager()->load_resource<resource::Bitmap>("");

  _stage = ERROR;
}

}// namespace e00::impl
