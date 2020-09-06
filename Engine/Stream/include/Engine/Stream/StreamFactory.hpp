#pragma once

#include <memory>
#include <Engine/Stream/Stream.hpp>

namespace e00::sys {
class StreamFactory {
public:
  virtual ~StreamFactory() = default;

  virtual std::unique_ptr<Stream> open_stream(const std::string &fileName) = 0;
};
}// namespace sys
