#pragma once

#include <string>
#include <vector>

#include <Engine/RuntimeConfig/Entry.hpp>
#include <Engine/RuntimeConfig/ParserListener.hpp>
#include "../ResourceConfiguration.hpp"

namespace e00::impl {
class ResourcePackConfiguration : public cfg::ParserListener {
public:
  std::string _id;
  std::string _name;
  std::vector<ResourceConfiguration> _configurations;

  ResourcePackConfiguration() : _id{}, _name{} {}

  std::error_code handle_entry(const cfg::Entry &configuration_entry) override;
};
}
