#pragma once

#include <list>
#include <string_view>
#include <string>
#include <memory>
#include <utility>

#include <Engine/ResourceSystem/PackIndex.hpp>
#include <Engine/ResourceSystem/ResourcePtr.hpp>
#include <Engine/ResourceSystem/Resource.hpp>
#include <Engine/ResourceSystem/Resources/Map.hpp>
#include <Engine/ResourceSystem/Resources/Bitmap.hpp>
#include <Engine/ResourceSystem/Resources/Sprite.hpp>

#include <Logger/Logger.hpp>
#include <Engine/Stream/StreamFactory.hpp>

namespace e00::resource {
template<typename T>
struct resource_to_info_type {};

template<>
struct resource_to_info_type<Map> {
  static constexpr info::Type info_type = info::Type::map;
};

template<>
struct resource_to_info_type<Bitmap> {
  static constexpr info::Type info_type = info::Type::bitmap;
};

class Manager {
  friend class ResourcePtrData;

  impl::Logger _logger;

  // Index of known resources and their packs
  std::list<PackIndex> _resources_index;

  [[nodiscard]] std::unique_ptr<Resource> load_resource(const std::string &resource_name, info::Type type);

  void add_reference(const ResourcePtrData *data);

  void remove_reference(const ResourcePtrData *data);

public:
  Manager();

  ~Manager();

  /**
   * Loads a resource
   *
   * @tparam T the resource type
   * @param resource_name the resource to load
   * @return the resource or null
   */
  template<typename T>
  ResourcePtr<T> load_resource(const std::string &resource_name) {
    static_assert(std::is_convertible<T, Resource>::value, "Class must be subclass of Resource");
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
      "T must be a non-const, non-volatile value_type");

    if (auto r = load_resource(resource_name, resource_to_info_type<T>::info_type)) {
      return ResourcePtr<T>(this, static_cast<T *>(r.release()));
    }

    return nullptr;
  };

  /**
   * Adds a pack to the global knowledge base
   *
   * @param pack_loader the pack loader
   * @return error code, if any
   */
  std::error_code add_pack(std::unique_ptr<sys::StreamFactory> &&pack_loader);
};


}// namespace e00::resource
