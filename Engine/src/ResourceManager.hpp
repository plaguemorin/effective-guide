#pragma once

#include <map>
#include <memory>
#include <string>

#include <Engine/CommonTypes.hpp>
#include <Engine/ResourceStream.hpp>
#include <Engine/Resource/Resource.hpp>
#include <Engine/System/ResourceStreamLoader.hpp>
#include <Engine/RuntimeConfig/ParserListener.hpp>

#include "Logger.hpp"

#include "ResourceConfiguration.hpp"
#include "Error.hpp"

namespace e00::impl {
class ResourceManager {
  impl::Logger _logger;
  sys::ResourceStreamLoader* _loader;

  // Resources configuration
  std::map<std::string, impl::ResourceConfiguration> _known_resources;

public:
  explicit ResourceManager(sys::ResourceStreamLoader *stream_loader);

  ~ResourceManager();

  /**
   *
   * @param pack_loader
   * @return
   */
  std::error_code add_pack(sys::ResourceStreamLoader *pack_loader);

  /**
   * Parses a configuration file
   * Needs at least `set_resource_loader` to be called with a valid stream loader
   *
   * @param file_name the configuration file to parse
   * @param listener the object to callback to for each value parsed
   *
   * @return the error code if the the file was not parsed successfully
   */
  std::error_code parse_configuration(const std::string &file_name, cfg::ParserListener *listener);

  /**
   * Unconditionally loads a resource, the callee is responsible for casting to the right type
   *
   * @param resource_name the resource to load
   * @param type expected resource type id
   * @return the resource or nullptr
   */
  std::unique_ptr<resource::Resource> load_resource(const std::string &resource_name, type_t type);

  /**
   * Unconditionally loads a resource
   *
   * @tparam T a resource type
   * @param resource_name the resource to load
   * @return a T resource, loaded
   */
  template<typename T>
  std::unique_ptr<T> load_resource(const std::string &resource_name) {
    static_assert(std::is_convertible<T *, resource::Resource *>::value, "Class must be subclass of Resource");

    const auto res = load_resource(resource_name, type_id<T>());
    if (res != nullptr) {
      return std::unique_ptr<T>(static_cast<T *>(res.get()));
    }
    return nullptr;
  }
};
}// namespace e00::impl
