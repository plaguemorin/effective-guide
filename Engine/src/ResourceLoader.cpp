#include "ResourceLoader.hpp"

#include <Engine/string_view_utils.hpp>
#include <Engine/Configuration/ParserListener.hpp>
#include <Engine/Configuration/Parser.hpp>

#include <Engine/Resources/Tileset.hpp>
#include <Engine/Resources/Bitmap.hpp>

#include "GIFLDR.hpp"

namespace e00::impl {
std::unique_ptr<resource::Resource> ResourceLoader::load(const std::string &res_name, type_t res_type) {
  if (res_type == type_id<resource::Tileset>()) {
    return load_tileset(res_name);
  } else if (res_type == type_id<resource::Bitmap>()) {
    return load_bitmap(res_name);
  }

  return nullptr;
}

std::unique_ptr<resource::Tileset> ResourceLoader::load_tileset(const std::string &res_name) {
  class TilesetParser : public cfg::ParserListener {
  public:
    std::string data;
    uint16_t tile_height;
    uint16_t tile_width;

    std::error_code handle_entry(const cfg::Entry &configuration_entry) override {
      if (configuration_entry.section.empty()) {
        if (configuration_entry.name == "Data") {
          data = configuration_entry.value;
          return {};
        }

        if (configuration_entry.name == "TileHeight") {
          return stoi_fast(configuration_entry.value, tile_height);
        }

        if (configuration_entry.name == "TileWidth") {
          return stoi_fast(configuration_entry.value, tile_width);
        }
      }

      return {};
    }
  };

  _logger.info(SourceLocation(), "Loading tileset {}", res_name);
  // Build our config listener
  TilesetParser listener;

  // Load the configuration
  if (auto config_stream = _stream_factory->open_stream(res_name + ".ini")) {
    if (auto ec = cfg::parse(config_stream, &listener)) {
      // Something went wrong !
      _logger.error(SourceLocation(), "Failed to load tileset configuration: {}", ec.message());
      return nullptr;
    }
  } else {
    _logger.error(SourceLocation(), "Failed to find {}.ini for tileset configuration", res_name);
    return nullptr;
  }

  // Load the backing bitmap
  if (listener.data.empty()) {
    _logger.error(SourceLocation(), "No backing bitmap (\"Data\" value) for tileset {}", res_name);
    return nullptr;
  }

  return std::unique_ptr<resource::Tileset>(new resource::Tileset(
    res_name,
    load_bitmap(listener.data),
    listener.tile_width,
    listener.tile_height));
}

std::unique_ptr<resource::Bitmap> ResourceLoader::load_bitmap(const std::string &res_name) {
  _logger.info(SourceLocation(), "Loading bitmap {}", res_name);

  // TODO: Is it a GIF or PNG ?

  if (auto stream = _stream_factory->open_stream(res_name)) {
    auto vector = load_gif_from_stream(std::move(stream));
    if (vector.empty()) {
      return nullptr;
    }
    return std::move(vector.front());
  }

  return nullptr;
}
}// namespace e00::impl
