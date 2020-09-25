#include "RenderWindow.hpp"


RenderWindow::RenderWindow(Win32System &system, const e00::Vec2I &size)
  : e00::sys::OutputScreen(size, BackingType::HARDWARE),
    Win32Window(system.instance(), L"E00RenderWnd", L"E00", size.x, size.y),
    _system(system) {
  _raw_screen.resize(size.total());
}

std::unique_ptr<e00::resource::Bitmap> RenderWindow::convert_to_hardware(const std::unique_ptr<e00::resource::Bitmap> &from) {
  return nullptr;
}

uint8_t RenderWindow::get_pixel(const e00::Vec2<int> &point) const {
  const auto pos = point.x + (point.y * width());
  return _raw_screen.at(pos);
}

void RenderWindow::set_pixel(const e00::Vec2<int> &point, uint8_t color) {
  const auto pos = point.x + (point.y * width());
  _raw_screen.at(pos) = color;
  auto c = _palette[color];
  SetPixel(point.x, point.y, c.red, c.green, c.blue);
}

void RenderWindow::set_color(uint8_t number, e00::Color color) {
  _palette[number] = color;
}

LRESULT RenderWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return Win32Window::HandleMessage(uMsg, wParam, lParam);
}

void RenderWindow::blit_bitmap(const e00::Vec2<int> &from, const e00::resource::Bitmap *source) {
  OutputScreen::blit_bitmap(from, source);
}
