#pragma once

namespace e00::sys {
namespace eventdata {
  namespace detail {
    struct ButtonEventData {
      enum Type : unsigned char {
        PRESSED,
        RELEASED
      };

      Type type;
      unsigned short button_id;

      constexpr ButtonEventData() : type(PRESSED), button_id(0) {}

      constexpr ButtonEventData(Type t, unsigned short bid) : type(t), button_id(bid) {}

      [[nodiscard]] constexpr int hash() const {
        return button_id
               + (type << 16u);
      }
    };

    struct AxisMovementEventData {
      unsigned char axis;
      short delta;

      constexpr AxisMovementEventData() : axis(0), delta(0) {}

      constexpr AxisMovementEventData(unsigned char a, short d) : axis(a), delta(d) {}

      [[nodiscard]] constexpr int hash() const {
        return static_cast<unsigned short>(delta)
               + (axis << 16u);
      }
    };
  }// namespace detail

  namespace helper {
    // Use directly and
    template<char axis>
    struct AxisHelper {
      int delta = 0;

      [[nodiscard]] detail::AxisMovementEventData GetAndReset() {
        const auto ret = detail::AxisMovementEventData(axis, (short)delta);
        delta = 0;
        return ret;
      }

      void add(short d) { delta = delta + d; }

      explicit operator bool() const { return delta != 0; }
    };

    template<unsigned short button_id>
    struct ButtonHelper {
      bool has_data = false;
      detail::ButtonEventData::Type type = detail::ButtonEventData::RELEASED;

      void set(detail::ButtonEventData::Type set_type) {
        has_data = true;
        type = set_type;
      }

      [[nodiscard]] detail::ButtonEventData GetAndReset() {
        detail::ButtonEventData ret(type, button_id);
        has_data = false;
        return ret;
      }

      explicit operator bool() const { return has_data; }
    };
  }// namespace helper

  struct KeyboardEventData {
    enum Type : int {
      PRESSED,
      RELEASED
    };

    enum Key : int {
      KEY_UNKNOWN,

      KEY_A,
      KEY_B,
      KEY_C,
      KEY_D,
      KEY_E,
      KEY_F,
      KEY_G,
      KEY_H,
      KEY_I,
      KEY_J,
      KEY_K,
      KEY_L,
      KEY_M,
      KEY_N,
      KEY_O,
      KEY_P,
      KEY_Q,
      KEY_R,
      KEY_S,
      KEY_T,
      KEY_U,
      KEY_V,
      KEY_W,
      KEY_X,
      KEY_Y,
      KEY_Z,
      KEY_0,
      KEY_1,
      KEY_2,
      KEY_3,
      KEY_4,
      KEY_5,
      KEY_6,
      KEY_7,
      KEY_8,
      KEY_9,
      KEY_0_PAD,
      KEY_1_PAD,
      KEY_2_PAD,
      KEY_3_PAD,
      KEY_4_PAD,
      KEY_5_PAD,
      KEY_6_PAD,
      KEY_7_PAD,
      KEY_8_PAD,
      KEY_9_PAD,
      KEY_F1,
      KEY_F2,
      KEY_F3,
      KEY_F4,
      KEY_F5,
      KEY_F6,
      KEY_F7,
      KEY_F8,
      KEY_F9,
      KEY_F10,
      KEY_F11,
      KEY_F12,
      KEY_ESC,
      KEY_TILDE,
      KEY_MINUS,
      KEY_EQUALS,
      KEY_BACKSPACE,
      KEY_TAB,
      KEY_OPENBRACE,
      KEY_CLOSEBRACE,
      KEY_ENTER,
      KEY_COLON,
      KEY_QUOTE,
      KEY_BACKSLASH,
      KEY_COMMA,
      KEY_STOP,
      KEY_SLASH,
      KEY_SPACE,
      KEY_INSERT,
      KEY_DEL,
      KEY_HOME,
      KEY_END,
      KEY_PGUP,
      KEY_PGDN,
      KEY_LEFT,
      KEY_RIGHT,
      KEY_UP,
      KEY_DOWN,
      KEY_SLASH_PAD,
      KEY_ASTERISK,
      KEY_MINUS_PAD,
      KEY_PLUS_PAD,
      KEY_DEL_PAD,
      KEY_ENTER_PAD,
      KEY_PRTSCR,
      KEY_PAUSE,
      KEY_ABNT_C1,
      KEY_YEN,
      KEY_KANA,
      KEY_CONVERT,
      KEY_NOCONVERT,
      KEY_AT,
      KEY_CIRCUMFLEX,
      KEY_COLON2,
      KEY_KANJI,

      KEY_SYSRQ,
      KEY_LSHIFT,
      KEY_RSHIFT,
      KEY_LCONTROL,
      KEY_RCONTROL,
      KEY_ALT,
      KEY_ALTGR,
      KEY_LWIN,
      KEY_RWIN,
      KEY_MENU,
      KEY_SCRLOCK,
      KEY_NUMLOCK,
      KEY_CAPSLOCK,
    };

    Type type;
    Key key;

    constexpr KeyboardEventData() : type(), key(KEY_UNKNOWN) {}

    constexpr KeyboardEventData(Type t, Key k) : type(t), key(k) {}

    constexpr explicit operator bool() const { return key == KEY_UNKNOWN; }

    [[nodiscard]] constexpr int hash() const { return key + (type << 24U); }

    constexpr static KeyboardEventData Pressed(Key k) { return KeyboardEventData(PRESSED, k); }
    constexpr static KeyboardEventData Released(Key k) { return KeyboardEventData(RELEASED, k); }
  };

  struct MouseEventData {
    enum Type : unsigned char {
      BUTTON,
      MOVEMENT
    };

    Type type;
    union {
      detail::ButtonEventData button_data;
      detail::AxisMovementEventData movement_data;
    };

    constexpr MouseEventData() : type(BUTTON), button_data() {}

    explicit MouseEventData(detail::ButtonEventData button_event_data) : type(BUTTON), button_data(button_event_data) {}

    explicit MouseEventData(detail::AxisMovementEventData movement_event_data) : type(MOVEMENT), movement_data(movement_event_data) {}

    [[nodiscard]] constexpr int hash() const {
      return (type << 24u)
             + (type == BUTTON ? button_data.hash() : movement_data.hash());
    }
  };

  struct JoystickEventData {
    enum Type : unsigned char {
      BUTTON,
      MOVEMENT
    };

    Type type;
    union {
      detail::ButtonEventData button_data;
      detail::AxisMovementEventData movement_data;
    };

    constexpr JoystickEventData() : type(BUTTON), button_data() {}

    constexpr explicit JoystickEventData(detail::ButtonEventData button_event_data) : type(BUTTON), button_data(button_event_data) {}

    constexpr explicit JoystickEventData(detail::AxisMovementEventData movement_event_data) : type(MOVEMENT), movement_data(movement_event_data) {}

    [[nodiscard]] constexpr int hash() const {
      return (type << 24u)
             + (type == BUTTON ? button_data.hash() : movement_data.hash());
    }
  };

  struct NoEventData {
  };
}// namespace eventdata

struct InputEvent {
  enum class Type {
    NONE,
    KEYBOARD,
    MOUSE,
    JOYSTICK,
  };

  const Type type;
  int id;

  union {
    eventdata::NoEventData no_data;
    eventdata::KeyboardEventData keyboard_data;
    eventdata::MouseEventData mouse_data;
    eventdata::JoystickEventData joystick_data;
  };

  // Allows quick `if (event)` types of check
  explicit operator bool() const { return type != Type::NONE; }

  // You can always construct a NONE event
  constexpr InputEvent()
    : type(Type::NONE), id(0), no_data() {}

  constexpr explicit InputEvent(eventdata::KeyboardEventData data)
    : type(Type::KEYBOARD), id(data.hash()), keyboard_data{ data } {}

  constexpr explicit InputEvent(eventdata::MouseEventData data)
    : type(Type::MOUSE), id(data.hash()), mouse_data{ data } {}

  constexpr explicit InputEvent(eventdata::JoystickEventData data)
    : type(Type::JOYSTICK), id(data.hash()), joystick_data{ data } {}
};
}// namespace sys
