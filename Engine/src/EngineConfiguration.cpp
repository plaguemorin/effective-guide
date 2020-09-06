#include "EngineConfiguration.hpp"

#include <Engine/string_view_utils.hpp>

#include "configuration_error.hpp"

namespace e00::impl {
std::error_code EngineConfiguration::handle_entry(const cfg::Entry &value) {
  // Special "setup" section ?
  if (value.section == "setup") {
    // Everything gets copied into a special setup array
    _setup.insert(std::make_pair(value.name, value.value));
    return {};
  }

  if (value.section == "General") {
    if (value.name == "EnableDiagnostics") {
      _enable_diagnostics = string_view_to_bool(value.value);
      return {};
    }

    if (value.name == "StartMap") {
      _start_map = std::string(value.value);
      return {};
    }

    if (value.name == "Packs") {
      std::vector<std::string_view> packs_tmp;
      split_comma_separated(value.value, std::back_inserter(packs_tmp));

      for (const auto &pack_str : packs_tmp) {
        _packs.emplace_back(pack_str.begin(), pack_str.end());
      }

      return {};
    }
  }

  return {};
}

}// namespace e00::impl
