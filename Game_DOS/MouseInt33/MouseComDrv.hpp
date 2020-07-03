#pragma once

#include <Engine/System/InputSystem.hpp>

class MouseComDrv : public e00::sys::InputSystem {
  bool _has_mouse;
  bool _has_mouse_wheel;
  int _wheel_pos;

  e00::sys::eventdata::helper::AxisHelper<0> _axis_x;
  e00::sys::eventdata::helper::AxisHelper<1> _axis_y;

  e00::sys::eventdata::helper::ButtonHelper<0> _left_btn;
  e00::sys::eventdata::helper::ButtonHelper<1> _right_btn;
  e00::sys::eventdata::helper::ButtonHelper<2> _middle_btn;

public:
  MouseComDrv();
  ~MouseComDrv() override;

  [[nodiscard]] bool has_mouse() const { return _has_mouse; }

  void mouse_update();

  [[nodiscard]] e00::sys::InputEvent next_event() override {
    if (_axis_x) {
      return e00::sys::InputEvent(e00::sys::eventdata::MouseEventData(_axis_x.GetAndReset()));
    }

    if (_axis_y) {
      return e00::sys::InputEvent(e00::sys::eventdata::MouseEventData(_axis_y.GetAndReset()));
    }

    if (_left_btn) {
      return e00::sys::InputEvent(e00::sys::eventdata::MouseEventData(_left_btn.GetAndReset()));
    }

    if (_right_btn) {
      return e00::sys::InputEvent(e00::sys::eventdata::MouseEventData(_right_btn.GetAndReset()));
    }

    if (_middle_btn) {
      return e00::sys::InputEvent(e00::sys::eventdata::MouseEventData(_middle_btn.GetAndReset()));
    }

    return {};
  }
};
