#pragma once

#include <memory>

#include "Info.hpp"
#include <Engine/ResourceSystem/Resources/Map.hpp>

#include <Engine/Stream/StreamFactory.hpp>

namespace e00::resource::loader {
std::unique_ptr<Map> load_map(const std::unique_ptr<sys::StreamFactory>& stream_factory, const Info& info, std::error_code& load_error);
}
