#pragma once

#include <memory>
#include <system_error>

#include <Engine/Stream/Stream.hpp>
#include <Engine/Resources/Bitmap.hpp>

namespace e00::impl {
std::vector<std::unique_ptr<resource::Bitmap>> load_bmp_from_stream(const std::unique_ptr<Stream>& stream);

enum class BmpLoaderErrorCode {
  not_valid_bmp = 1,

};

std::error_code make_error_code(BmpLoaderErrorCode);
}

namespace std {
template<>
struct is_error_code_enum<e00::impl::BmpLoaderErrorCode> : true_type {};
}// namespace std

