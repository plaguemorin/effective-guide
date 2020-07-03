#pragma once

#include <unordered_map>

#include <Engine/RuntimeConfig/ParserListener.hpp>
#include <Engine/Math.hpp>

namespace e00::impl {
class EngineConfiguration : public cfg::ParserListener {
public:
  std::error_code handle_entry(const cfg::Entry &value) override;

  std::unordered_map<std::string, std::string> _setup;
};

}// namespace e00::impl
