#pragma once

#include <dpmi.h>

#include <Engine/System/InputSystem.hpp>

#include "KeyboardEventHandlerResult.hpp"
#include "KeyboardEventPauseHandler.hpp"
#include "KeyboardEventExtendedHandler.hpp"
#include "KeyboardEventNormalHandler.hpp"

class KeyboardEventHandler : public e00::sys::InputSystem {
  KeyboardEventPauseHandler _pause_handler;
  KeyboardEventExtendedHandler _extended_handler;
  KeyboardEventNormalHandler _normal_handler;

  _go32_dpmi_seginfo keyboardOldISR, keyboardNewISR;
  int next_key;

public:
  KeyboardEventHandler() noexcept;

  ~KeyboardEventHandler() noexcept override;

  e00::sys::InputEvent next_event() override;
};
