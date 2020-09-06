#include "ResourcePackConfiguration.hpp"
#include "ResourceSectionStringConvert.hpp"

#include <Engine/string_view_utils.hpp>

namespace {
using namespace e00::resource;

info::Map::DataType ToDataFormat(const std::string_view &value) {
  if (value == "Text") {
    return info::Map::DataType::text;
  }

  return info::Map::DataType::unknown;
}

info::MapPosition ToMapPosition(const std::string_view &view) {
  // Format is <MAP ID>$x,y
  const auto hash = view.find_first_of('#');
  const auto coma = view.find_last_of(',');

  info::MapPosition ret;

  // Make sure we're parsing something valid
  if (coma != std::string_view::npos
      && hash != std::string_view::npos
      && coma > hash) {
    ret.map_name = e00::trim(view.substr(0, hash));
    e00::stoi_fast(e00::trim(view.substr(hash + 1, coma - hash - 1)), ret.position_x);
    e00::stoi_fast(e00::trim(view.substr(coma + 1)), ret.position_y);
  }

  return ret;
}

std::error_code HandleConfigParse(info::Map &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "Tileset") {
    config.tileset = config_value;
  }

  if (config_name == "Data") {
    config.data = config_value;
  }

  if (config_name == "DataFormat") {
    config.data_type = ToDataFormat(config_value);
  }

  if (config_name == "Width") {
    e00::stoi_fast(config_value, config.width);
  }

  if (config_name == "Height") {
    e00::stoi_fast(config_value, config.height);
  }

  return {};
}

std::error_code HandleConfigParse(info::Actor &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "Position")
    config.position = ToMapPosition(config_value);
  if (config_name == "Sprite")
    config.sprite = config_value;
  if (config_name == "Angle")
    e00::stoi_fast(config_value, config.angle);

  return {};
}

std::error_code HandleConfigParse(info::Portal &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "From")
    config.from_position = ToMapPosition(config_value);
  if (config_name == "Target")
    config.to_position = ToMapPosition(config_value);
  if (config_name == "FromDisplayName")
    config.from_name_string_id = config_value;

  return {};
}

std::error_code HandleConfigParse(info::Sprite &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "Width")
    e00::stoi_fast(config_value, config.width);

  if (config_name == "Height")
    e00::stoi_fast(config_value, config.height);


  return {};
}

std::error_code HandleConfigParse(info::Tileset &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "TileWidth")
    e00::stoi_fast(config_value, config.width);

  if (config_name == "TileHeight")
    e00::stoi_fast(config_value, config.height);

  if (config_name == "Data")
    config.bitmap = config_value;

  return {};
}

std::error_code HandleConfigParse(info::Bitmap &config, const std::string_view &config_name, const std::string_view &config_value) {
  (void)config;
  (void)config_name;
  (void)config_value;

  return {};
}


std::error_code handle_configuration(Info &info, const std::string_view &name, const std::string_view &value) {
  switch (info.type()) {
    case info::Type::unknown: break;
    case info::Type::map:
      return HandleConfigParse(info.map(), name, value);
    case info::Type::actor:
      return HandleConfigParse(info.actor(), name, value);
    case info::Type::portal:
      return HandleConfigParse(info.portal(), name, value);
    case info::Type::sprite:
      return HandleConfigParse(info.sprite(), name, value);
    case info::Type::tileset:
      return HandleConfigParse(info.tileset(), name, value);
    case info::Type::bitmap:
      return HandleConfigParse(info.bitmap(), name, value);
  }
  return {};
}


}// namespace

namespace e00::resource::impl {
ResourcePackConfiguration::ResourcePackConfiguration(const std::string_view &resource_name, info::Type type)
  : _resource_name(resource_name),
    _type(type),
    _info(std::string(resource_name), type) {
}

ResourcePackConfiguration::~ResourcePackConfiguration() {
}

std::error_code ResourcePackConfiguration::handle_entry(const cfg::Entry &configuration_entry) {
  // We are only interested in our section
  const auto pair = ParseIdType(configuration_entry.section);
  if (_resource_name == pair.first && _type == pair.second) {
    return handle_configuration(_info, trim(configuration_entry.name), trim(configuration_entry.value));
  }

  return {};
}


const Info& ResourcePackConfiguration::make_info() const {
  return _info;
}

}// namespace e00::resource::impl
