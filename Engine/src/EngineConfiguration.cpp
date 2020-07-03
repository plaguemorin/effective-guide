#include "EngineConfiguration.hpp"

#include <sstream>
#include <algorithm>

#include "string_view_utils.hpp"
#include "configuration_error.hpp"

namespace e00::impl {
std::error_code EngineConfiguration::handle_entry(const cfg::Entry &value) {
  // Special "setup" section ?
  if (value.section == "setup") {
    // Everything gets copied into a special setup array
    _setup.insert(std::make_pair(value.name, value.value));
    return {};
  }

  return {};
}

}// namespace e00::impl
