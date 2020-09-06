#pragma once

#include <Engine/Configuration/ParserListener.hpp>
#include <Engine/ResourceSystem/PackIndex.hpp>

namespace e00::resource::impl {
class PackIndexMaker : public cfg::ParserListener {
  PackIndex &_index;

public:
  explicit PackIndexMaker(e00::resource::PackIndex &index) : _index(index) {}
  ~PackIndexMaker() override = default;

  std::error_code handle_entry(const e00::cfg::Entry &configuration_entry) override;
};
}// namespace e00::resource::impl
