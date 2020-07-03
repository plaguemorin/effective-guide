#pragma once

#include <string>

#include <Engine/RuntimeConfig/ParserListener.hpp>

#include "../Resource/Map.hpp"
#include "../Logger.hpp"

namespace e00::impl {
class MapConfigParser : public cfg::ParserListener {
  Logger _logger;
  uint8_t _width;
  uint8_t _height;
  uint8_t _num_layers;
  std::string _tileset;

public:
  MapConfigParser();
  ~MapConfigParser() override = default;
  std::error_code handle_entry(const cfg::Entry &configuration_entry) override;
  Map build() const;
  [[nodiscard]] const std::string &tileset() const { return _tileset; }
};
}// namespace e00impl
