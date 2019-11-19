
#include "Engine.hpp"

#include <fstream>

Engine::Engine(System& system)
        : _system(system),
          _paused(false),
          _game_state(State::STARTUP) {
}

Engine::~Engine() {

}

void Engine::Ticker() {
  switch (_game_state) {
    case State::IN_LEVEL:
      // Update actors
      // Update status bar
      // Update map
      // HUD
      break;
  }
}

void Engine::TryRunTick() {

}
