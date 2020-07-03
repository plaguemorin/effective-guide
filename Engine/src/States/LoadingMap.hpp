#pragma once

#include <Engine/Math.hpp>

#include "State.hpp"
#include "../Resource/Map.hpp"
#include "../Tileset.hpp"
#include "../Loaders/MapConfigParser.hpp"

namespace e00::impl {
class Loading_Map : public State {
  enum Stage {
    LOAD_MAP,
    LOAD_TILESET,
    COMPUTE,
    ERROR
  };

  Logger _logger;
  const std::string _map_path;
  MapConfigParser _map_parser;
  Stage _stage;

  void load_map();
  void load_tileset();

public:
  explicit Loading_Map(std::string map_name);

  Loading_Map(const Loading_Map &) = delete;

  Loading_Map(Loading_Map &&) = delete;

  virtual ~Loading_Map() = default;

  Loading_Map &operator=(const Loading_Map &) = delete;

  Loading_Map &operator=(Loading_Map &&) = delete;

  State *update(const std::chrono::milliseconds &delta_since_last_update) override;

  bool needs_steady_updates() const override { return false; }

  void render() override;
};

}// namespace e00impl
