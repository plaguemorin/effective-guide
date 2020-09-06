#pragma once

#include <system_error>
#include <Engine/System/InputEvent.hpp>

namespace e00::sys {
class InputSystem {
public:
  InputSystem() = default;
  InputSystem(const InputSystem &) = delete;
  InputSystem(InputSystem &&) noexcept = delete;

  virtual ~InputSystem() = default;

  InputSystem &operator=(const InputSystem &) = delete;
  InputSystem &operator=(InputSystem &&) = delete;

  /**
   * Called by the engine before any calls to next_event
   *
   * @return error code if initialization failed
   */
  virtual std::error_code init() { return {}; }

  virtual InputEvent next_event() = 0;
};
}// namespace e00::sys
