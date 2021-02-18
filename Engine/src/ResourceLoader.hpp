#pragma once

#include <memory>
#include <string>

#include <Engine/Stream/StreamFactory.hpp>
#include <Engine/Resource.hpp>
#include <Logger/Logger.hpp>

namespace e00::impl {
class ResourceLoader {
  Logger _logger;
  const std::unique_ptr<sys::StreamFactory> &_stream_factory;

  std::unique_ptr<resource::Tileset> load_tileset(const std::string& res_name);
  std::unique_ptr<resource::Bitmap> load_bitmap(const std::string& res_name);

public:
  explicit ResourceLoader(const std::unique_ptr<sys::StreamFactory> &stream_factory)
    : _logger("RezLoader"),
      _stream_factory(stream_factory) {}

  std::unique_ptr<resource::Resource> load(const std::string &res_name, type_t res_type);
};
}// namespace e00::impl
