#include "KeyboardEventHandler.hpp"

#include <pc.h>
#include <go32.h>
#include <dos.h>

constexpr auto KEYBOARD_BUFFER_SIZE = 512;
constexpr auto KEYBOARD_ISR = 9;

struct KeyboardHandlerCriticalData {
  int intr_next_index;
  uint8_t keys[KEYBOARD_BUFFER_SIZE];
};

namespace {
volatile struct KeyboardHandlerCriticalData critical_data;

void keyboard_handler() {
  auto ints = disable();
  uint8_t code = inportb(0x60);

  int i = critical_data.intr_next_index;
  critical_data.keys[i] = code;

  if (i >= KEYBOARD_BUFFER_SIZE - 1) {
    critical_data.intr_next_index = 0;
  } else {
    critical_data.intr_next_index = i + 1;
  }

  outportb(0x20, 0x20);
  if (ints) enable();
}
}// namespace

KeyboardEventHandler::KeyboardEventHandler() noexcept
  : next_key(0) {
  _go32_dpmi_get_protected_mode_interrupt_vector(KEYBOARD_ISR, &keyboardOldISR);

  critical_data.intr_next_index = 0;

  _go32_dpmi_lock_data((void *)&critical_data, sizeof(critical_data));
  _go32_dpmi_lock_code((void *)&keyboard_handler, 4096);
  keyboardNewISR.pm_offset = (unsigned long)&keyboard_handler;
  keyboardNewISR.pm_selector = (unsigned short)_my_cs();

  _go32_dpmi_allocate_iret_wrapper(&keyboardNewISR);
  _go32_dpmi_set_protected_mode_interrupt_vector(KEYBOARD_ISR, &keyboardNewISR);
}

KeyboardEventHandler::~KeyboardEventHandler() noexcept {
  _go32_dpmi_set_protected_mode_interrupt_vector(KEYBOARD_ISR, &keyboardOldISR);
  _go32_dpmi_free_iret_wrapper(&keyboardNewISR);
}

e00::sys::InputEvent KeyboardEventHandler::next_event() {
  if (next_key >= KEYBOARD_BUFFER_SIZE) {
    next_key = 0;
  }

  if (critical_data.intr_next_index > next_key) {
    const auto scancode = critical_data.keys[next_key++];
    if (scancode == 0) {
      return {};
    }

    // the pause sequence needs to be first
    const auto pause_result = _pause_handler.handle(scancode);
    if (!pause_result.should_continue_processing) {
      if (pause_result.has_event_data) {
        return e00::sys::InputEvent(pause_result.event);
      }

      return {};
    }

    // other extended keys will start with 0xE0
    const auto extended_result = _extended_handler.handle(scancode);
    if (!extended_result.should_continue_processing) {
      if (extended_result.has_event_data) {
        return e00::sys::InputEvent(extended_result.event);
      }

      return {};
    }

    // Normal keys!
    return e00::sys::InputEvent(_normal_handler.handle(scancode).event);
  }

  return {};
}
