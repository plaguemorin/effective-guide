#pragma once

#include <array>
#include <cstdint>

#include <Engine/Math.hpp>

namespace thing {
struct State {
  constexpr static uint16_t NO_NEXT_STATE = 0xFFFF;

  uint16_t id;
  uint16_t duration;

  uint16_t next_state;
};

class Info {
  constexpr static auto MAX_STATES = 64;

  uint16_t _id;
  PointI _position;
  std::array<State, MAX_STATES> _states;
  uint16_t _state_count;

  uint16_t _initial_health;

  // Known state lists
  uint16_t _initial_state; // Spawned with this state
  uint16_t _see_state;     // State to transition to when the THING sees the player
  uint16_t _pain_state;    // State to transition to when the THING gets hurt
  uint16_t _death_state;   // State to transition to when health reaches 0


  template<int count, typename... Args>
  void add_state(State aState, Args&& ... states) {
    _states[count] = aState;
    add_state<count + 1>(std::forward<Args>(states)...);
  }

  template<int count>
  void add_state() {}

public:
  template<typename... States>
  explicit Info(uint16_t id, State aState, States&& ... states)
          : _id(id),
            _position(0, 0),
            _states{0},
            _state_count(sizeof...(States) + 1) {
    static_assert(sizeof...(States) + 1 < MAX_STATES);
    add_state<0>(aState, std::forward<States>(states)...);
  }

  ~Info() = default;

  uint16_t initial_state() const { return _initial_state; }

  uint16_t count_states() const { return _state_count; }

  uint16_t initial_health() const { return _initial_health; }
};
}
