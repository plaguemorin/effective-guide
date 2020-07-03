#pragma once

#include <Engine/System/InputEvent.hpp>

struct KeyboardEventHandlerResult {
  const bool should_continue_processing;
  const bool has_event_data;
  const e00::sys::eventdata::KeyboardEventData event;

  constexpr explicit KeyboardEventHandlerResult(bool s) : should_continue_processing(s), has_event_data(false), event() {}

  constexpr KeyboardEventHandlerResult(e00::sys::eventdata::KeyboardEventData event_data) : should_continue_processing(false), has_event_data(true), event(event_data) {}
};
