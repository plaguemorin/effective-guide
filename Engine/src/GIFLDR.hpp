#pragma once

#include <memory>
#include <system_error>

#include <Engine/Stream/Stream.hpp>
#include <Engine/Resources/Bitmap.hpp>

namespace e00::impl {
std::unique_ptr<resource::Bitmap> load_gif_from_stream(const std::unique_ptr<Stream>& stream);

enum class GifLoaderErrorCode {
  not_valid_gif = 1,

};

std::error_code make_error_code(GifLoaderErrorCode);
}

namespace std {
template<>
struct is_error_code_enum<e00::impl::GifLoaderErrorCode> : true_type {};
}// namespace std

