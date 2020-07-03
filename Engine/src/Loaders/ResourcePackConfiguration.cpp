#include "ResourcePackConfiguration.hpp"

#include "../configuration_error.hpp"
#include "../string_view_utils.hpp"

namespace {
using namespace e00::impl;

ResourceConfiguration::ConfigurationType ToType(const std::string_view &str) {
  if (str == "Map")
    return ResourceConfiguration::ConfigurationType::map;
  if (str == "Actor")
    return ResourceConfiguration::ConfigurationType::actor;
  if (str == "Portal")
    return ResourceConfiguration::ConfigurationType::portal;

  return ResourceConfiguration::ConfigurationType::unknown;
}

ResourceConfiguration::Map::DataType ToDataFormat(const std::string_view &value) {
  if (value == "Text")
    return ResourceConfiguration::Map::DataType::text;

  return ResourceConfiguration::Map::DataType::unknown;
}

ResourceConfiguration::MapPosition ToMapPosition(const std::string_view &view) {
  // Format is <MAP ID>$x,y
  const auto hash = view.find_first_of('#');
  const auto coma = view.find_last_of(',');

  ResourceConfiguration::MapPosition ret;

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

std::error_code HandleConfigParse(ResourceConfiguration::Map &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "Tileset")
    config.tileset = config_value;

  if (config_name == "Data")
    config.data = config_value;

  if (config_name == "DataFormat")
    config.data_type = ToDataFormat(config_value);

  if (config_name == "Width")
    e00::stoi_fast(config_value, config.width);

  if (config_name == "Height")
    e00::stoi_fast(config_value, config.height);

  return {};
}

std::error_code HandleConfigParse(ResourceConfiguration::Actor &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "Position")
    config.position = ToMapPosition(config_value);
  if (config_name == "Sprite")
    config.sprite = config_value;

  return {};
}

std::error_code HandleConfigParse(ResourceConfiguration::Portal &config, const std::string_view &config_name, const std::string_view &config_value) {
  if (config_name == "From")
    config.from_position = ToMapPosition(config_value);
  if (config_name == "Target")
    config.to_position = ToMapPosition(config_value);
  if (config_name == "FromDisplayName")
    config.from_name_string_id = config_value;

  return {};
}

std::error_code HandleConfigParse(ResourceConfiguration &config, const std::string_view &config_name, const std::string_view &config_value) {
  switch (config.type()) {
    case ResourceConfiguration::ConfigurationType::map: return HandleConfigParse(config.map(), config_name, config_value);
    case ResourceConfiguration::ConfigurationType::actor: return HandleConfigParse(config.actor(), config_name, config_value);
    case ResourceConfiguration::ConfigurationType::portal: return HandleConfigParse(config.portal(), config_name, config_value);
    case ResourceConfiguration::ConfigurationType::unknown: break;
  }

  return e00::make_error_code(e00::ConfigurationError::unknown_resource_type);
}

}// namespace

namespace e00::impl {
std::error_code ResourcePackConfiguration::handle_entry(const cfg::Entry &configuration_entry) {
  if (configuration_entry.section == "General") {
    if (configuration_entry.name == "ID") {
      _id = configuration_entry.value;
      return {};
    }

    if (configuration_entry.name == "Name") {
      _name = configuration_entry.value;
      return {};
    }
  }

  // Maybe it's a resource ?
  // It's format should be 'Type:Id'
  auto colon_pos = configuration_entry.section.find_first_of(':');
  if (colon_pos != std::string_view::npos) {
    // Make sure it's not an UNKNOWN type
    const auto type = ToType(configuration_entry.section.substr(0, colon_pos));
    if (type != ResourceConfiguration::ConfigurationType::unknown) {
      // It's a configuration!
      const auto id = configuration_entry.section.substr(colon_pos + 1);

      // Do we already know this resource ?
      for (auto &config : _configurations) {
        // Validate by ID
        if (config.id() == id) {
          // Make sure it's the same type as we think it should be
          if (config.type() != type) {
            return make_error_code(ConfigurationError::duplicate_resource);
          }

          // We found the matching ID and Type
          // Pass it off to the handler
          return HandleConfigParse(config, e00::trim(configuration_entry.name), e00::trim(configuration_entry.value));
        }
      }

      // We didn't find a config so create one
      ResourceConfiguration config(std::string(id), type);

      // Handle the config; don't insert it if there's an error parsing the line
      if (auto ec = HandleConfigParse(config, e00::trim(configuration_entry.name), e00::trim(configuration_entry.value))) {
        return ec;
      }

      // If we parsed the entry, add it to the list of known configurations
      _configurations.emplace_back(std::move(config));

      return {};
    }
  }

  return {};
}
}// namespace e00::impl
