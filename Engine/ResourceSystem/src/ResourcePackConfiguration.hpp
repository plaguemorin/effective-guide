#pragma once

#include <Engine/Configuration/ParserListener.hpp>
#include "Info.hpp"

namespace e00::resource::impl {
class ResourcePackConfiguration : public cfg::ParserListener {
  const std::string_view& _resource_name;
  const info::Type _type;
  Info _info;

public:
  ResourcePackConfiguration(const std::string_view& resource_name, info::Type type);

  ~ResourcePackConfiguration() override;

  std::error_code handle_entry(const cfg::Entry &configuration_entry) override;

  [[nodiscard]] const Info& make_info() const;
};
}
