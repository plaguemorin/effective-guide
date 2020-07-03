#pragma once

#include <system_error>

namespace e00 {

enum class ConfigurationError {
  too_many_parameters = 1,
  not_enough_parameters,
  duplicate_resource,
  unknown_resource_type,
};

std::error_code make_error_code(ConfigurationError);
}

namespace std {
template<>
struct is_error_code_enum<e00::ConfigurationError> : true_type {};
}
