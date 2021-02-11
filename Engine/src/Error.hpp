#pragma once

#include <system_error>

namespace e00 {
enum class EngineErrorCode {
  not_configured = 1,
  invalid_argument,
  failed_to_initialize_input,
  no_stream_loader_for_pak,
  too_many_input_systems,
  duplicate_resource,
  dependent_resource_not_found,
  invalid_pack,
  invalid_pack_configuration,
};

std::error_code make_error_code(EngineErrorCode);
}

namespace std {
template<>
struct is_error_code_enum<e00::EngineErrorCode> : true_type {};
}// namespace std
