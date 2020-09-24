#pragma once

#include <Engine/System/OutputScreen.hpp>
#include "Win32Window.hpp"
#include "Win32System.hpp"

class RenderWindow
  : public e00::sys::OutputScreen
  , public Win32Window {
  const Win32System &_system;

public:
  explicit RenderWindow(Win32System &system, const e00::Vec2I& size);
  ~RenderWindow() override = default;

  std::unique_ptr<e00::resource::Bitmap> convert_to_hardware(const std::unique_ptr<e00::resource::Bitmap> &from) override;
  uint8_t get_pixel(const e00::Vec2<int> &point) const override;
  void set_pixel(const e00::Vec2<int> &point, uint8_t color) override;

protected:
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};
