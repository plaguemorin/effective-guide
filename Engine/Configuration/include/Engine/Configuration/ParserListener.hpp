#pragma once

#include <system_error>
#include <Engine/Configuration/Entry.hpp>

namespace e00::cfg {
class ParserListener {
public:
  virtual ~ParserListener() = default;
  virtual std::error_code handle_entry(const Entry &configuration_entry) = 0;
};
}// namespace cfg
