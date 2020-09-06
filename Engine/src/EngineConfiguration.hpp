#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <Engine/Configuration/ParserListener.hpp>

namespace e00::impl {
class EngineConfiguration : public cfg::ParserListener {
public:
  std::error_code handle_entry(const cfg::Entry &value) override;

  std::unordered_map<std::string, std::string> _setup;
  bool _enable_diagnostics;
  std::vector<std::string> _packs;
  std::string _start_map;
};

}// namespace e00::impl
