#pragma once

#include <vector>
#include <Engine/System/OutputScreen.hpp>

#include "Win32Window.hpp"
#include "Win32System.hpp"

class RenderWindow final
  : public e00::sys::OutputScreen
  , public Win32Window {
  const Win32System &_system;
  e00::Palette<0xFF> _palette;
  std::vector<uint8_t> _raw_screen;
  HBITMAP _win32_bitmap;

public:
  explicit RenderWindow(Win32System &system, const e00::Vec2<uint16_t> &size);
  ~RenderWindow() final = default;

  std::unique_ptr<e00::resource::Bitmap> convert_to_hardware(const std::unique_ptr<e00::resource::Bitmap> &from) final;
  uint8_t get_pixel(const e00::Vec2<uint16_t> &point) const final;
  void set_pixel(const e00::Vec2<uint16_t> &point, uint8_t color) final;
  const e00::Palette<0xFF> *color_map() const final { return &_palette; }
  void set_color(uint8_t number, e00::Color color) final;
  void blit_bitmap(const e00::Vec2<uint16_t> &to, const Bitmap *source) final;

protected:
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) final;
};
