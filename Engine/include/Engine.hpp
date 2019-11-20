#pragma once

#include <Engine/System.hpp>

enum class State {
  STARTUP,
  IN_LEVEL,
};

class Engine {
  System* _system;
  bool _paused;
  State _game_state;

  void Ticker();

public:
  explicit Engine(System* system);

  ~Engine();

  void TryRunTick();
};
