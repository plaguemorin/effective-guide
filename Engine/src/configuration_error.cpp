#include "configuration_error.hpp"

namespace {
struct ConfigurationErrorCategory : std::error_category {
  const char *name() const noexcept override {
    return "configuration";
  }
  std::string message(int i) const override {
    switch (static_cast<e00::ConfigurationError>(i)) {
      case e00::ConfigurationError::too_many_parameters: return "too many parameters";
      case e00::ConfigurationError::not_enough_parameters: return "not enough parameters";
      case e00::ConfigurationError::duplicate_resource: return "duplicate resource id";
      case e00::ConfigurationError::unknown_resource_type: return "unknown resource type";
    }

    return {};
  }
};

const ConfigurationErrorCategory cfgErrCategory{};
}// namespace


namespace e00 {
std::error_code make_error_code(ConfigurationError e) {
  return std::error_code(static_cast<int>(e), cfgErrCategory);
}
}// namespace e00
