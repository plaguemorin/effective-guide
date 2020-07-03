#pragma once

#include <system_error>
#include <memory>

#include <Engine/ResourceStream.hpp>
#include <Engine/RuntimeConfig/ParserListener.hpp>

namespace e00::impl {
std::error_code parse_ini(const std::unique_ptr<ResourceStream> &stream, cfg::ParserListener *listener);
}
