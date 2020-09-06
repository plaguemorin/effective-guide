#pragma once

#include <list>
#include <string>
#include <memory>
#include <string_view>

#include <Engine/Stream/StreamFactory.hpp>
#include <Engine/ResourceSystem/Info/Type.hpp>

namespace e00::resource {
struct PackIndex {
  struct Locator {
    std::string name;
    info::Type type;

    Locator() noexcept : name{}, type(info::Type::unknown) {}
    Locator(std::string name_, info::Type type_) : name(std::move(name_)), type(type_) {}

    explicit operator bool() const { return !name.empty() && type != info::Type::unknown; }
  };

  std::string id;
  std::string name;

  std::unique_ptr<sys::StreamFactory> pack;
  std::list<Locator> resource_index;

  const static Locator end_;

  [[nodiscard]] const Locator &find(const std::string_view &resource_name, info::Type res_type) const {
    // We're just looking for name
    if (res_type == info::Type::unknown) {
      for (const auto &i : resource_index) {
        if (i.name == resource_name) {
          return i;
        }
      }

      return end_;
    }

    // We're looking for name AND type
    for (const auto &i : resource_index) {
      if (i.name == resource_name && i.type == res_type) {
        return i;
      }
    }

    return end_;
  }
};
}// namespace e00::resource
