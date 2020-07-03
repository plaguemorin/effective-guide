#pragma once

#include <system_error>

namespace e00 {
enum class EngineErrorCode {
  not_configured = 1,
  invalid_argument,
  failed_to_initialize_input,
  too_many_input_systems,
  duplicate_resource,
  dependent_resource_not_found,
};

std::error_code make_error_code(EngineErrorCode);
}

namespace std {
template<>
struct is_error_code_enum<e00::EngineErrorCode> : true_type {};
}// namespace std
