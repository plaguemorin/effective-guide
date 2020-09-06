#pragma once

#include <system_error>

namespace e00::resource {
enum class ResourceError {
  invalid_pack = 1,
  invalid_resource_type
};

std::error_code make_error_code(ResourceError);
}

namespace std {
template<>
struct is_error_code_enum<e00::resource::ResourceError> : true_type {};
}
