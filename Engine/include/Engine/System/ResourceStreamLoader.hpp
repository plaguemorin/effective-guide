#pragma once

#include <memory>
#include <Engine/ResourceStream.hpp>

namespace e00::sys {
class ResourceStreamLoader {
public:
  virtual ~ResourceStreamLoader() = default;

  virtual std::unique_ptr<ResourceStream> load_file(const std::string &fileName) = 0;
};
}// namespace sys
