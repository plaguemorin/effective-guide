#pragma once

#include <memory>
#include <string>

#include <Engine/Stream/StreamFactory.hpp>

namespace e00::sys {
class RootStreamFactory : public StreamFactory {
public:
  virtual std::unique_ptr<sys::StreamFactory> load_pack(const std::string& pack_name) = 0;
};

}// namespace e00::sys
