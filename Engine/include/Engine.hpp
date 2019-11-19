#pragma once

#include <array>

#include <Engine/System.hpp>
#include <Engine/TickCommand.hpp>
#include <Engine/ThingSystem/Info.hpp>
#include <Engine/ThingSystem/Manager.hpp>
#include <Engine/Map.hpp>

class Engine {
  constexpr static auto MAX_EVENTS = 127;
  constexpr static auto MAX_TICKEVENT = 127;

public:
  enum class State {
    STARTUP,
    IN_LEVEL,

  };

  explicit Engine(System& system);

  ~Engine();

  void TryRunTick();

private:
  System& _system;
  bool _paused;
  State _game_state;

  Map *_current_map;
  thing::Manager _thing_manager;
  std::array<TickCommand, MAX_TICKEVENT> _tick_commands;

  void Ticker();
};
