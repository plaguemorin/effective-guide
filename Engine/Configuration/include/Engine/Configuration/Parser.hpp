#pragma once

#include <memory>

#include <Engine/Configuration/Entry.hpp>
#include <Engine/Configuration/ParserListener.hpp>

#include <Engine/Stream/Stream.hpp>

namespace e00::cfg {
std::error_code parse(const std::unique_ptr<Stream> &stream, cfg::ParserListener *listener);
}
