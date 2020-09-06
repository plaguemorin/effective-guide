#include "PackIndexMaker.hpp"
#include "ResourceSectionStringConvert.hpp"

namespace e00::resource::impl {
std::error_code PackIndexMaker::handle_entry(const cfg::Entry &configuration_entry) {
  if (configuration_entry.section == "General") {
    if (configuration_entry.name == "ID") {
      _index.id = configuration_entry.value;
      return {};
    }

    if (configuration_entry.name == "Name") {
      _index.name = configuration_entry.value;
      return {};
    }
  }

  // Maybe it's a resource ?
  // It's format should be 'Type:Id'
  const auto pair = ParseIdType(configuration_entry.section);
  if (pair.second != info::Type::unknown) {
    // Do we already know ?
    if (!_index.find(pair.first, pair.second)) {
      // If we don't know it we need to add an entry about it
      _index.resource_index.emplace_back(std::string(pair.first), pair.second);
    }
  }

  return {};
}
}
