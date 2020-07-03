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
    case LOAD_MAP: load_map(); break;
    case LOAD_TILESET: load_tileset(); break;
    case COMPUTE: break;
    case ERROR: break;
  }

  return new Running_OnMap(_map_parser.build());
}
void Loading_Map::render() {

}

void Loading_Map::load_map() {
  // Load data of map_name
  if (auto ec = resource_manager()->parse_configuration(_map_path, &_map_parser)) {
    // TODO: Do something with the error
    _logger.error(SourceLocation(),  "Failed to load map: {}", ec.message());
    _stage = ERROR;
    return;
  }

  _stage = LOAD_TILESET;
  _logger.info(SourceLocation(), "Loaded map");
}


void Loading_Map::load_tileset() {
}

}// namespace e00impl
