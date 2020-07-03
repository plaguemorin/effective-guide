#include "MapConfigParser.hpp"

#include "../string_view_utils.hpp"

namespace e00::impl {
MapConfigParser::MapConfigParser()
  : _logger("MapConfigParser"),
    _width(0),
    _height(0),
    _num_layers(0) {
}

std::error_code MapConfigParser::handle_entry(const cfg::Entry &configuration_entry) {
  if (configuration_entry.section.empty()) {
    if (configuration_entry.name == "width") {
      return stoi_fast(configuration_entry.value, _width);
    }

    if (configuration_entry.name == "height") {
      return stoi_fast(configuration_entry.value, _height);
    }

    if (configuration_entry.name == "tileset") {
      _tileset = std::string(configuration_entry.value.begin(), configuration_entry.value.end());
    }

    if (configuration_entry.name == "layers") {
      return stoi_fast(configuration_entry.value, _num_layers);
    }

    return {};
  }

  if (configuration_entry.section == "layer") {
    if (configuration_entry.name == "line") {
      std::vector<uint16_t> tiles;
      execute_comma_separated(configuration_entry.value, [&tiles](const auto &sv) {
        uint16_t tile;
        stoi_fast(sv, tile);
        tiles.emplace_back(tile);
      });
    }
    return {};
  }

  return {};
}

Map MapConfigParser::build() const {
  _logger.info(SourceLocation(), "{}x{} on tileset {}", _width, _height, _tileset);
  auto map = Map(_width, _height);

  return map;
}
}// namespace e00impl
