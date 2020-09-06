#pragma once

#include <string_view>

namespace e00::sys {
class LoggerSink {
public:
  virtual ~LoggerSink() = default;

  virtual void info(const std::string_view &string) = 0;

  virtual void error(const std::string_view &string) = 0;
};
}// namespace sys
