#include "RenderWindow.hpp"


RenderWindow::RenderWindow(Win32System &system, const e00::Vec2I &size)
  : e00::sys::OutputScreen(size, BackingType::HARDWARE),
    Win32Window(system.instance(), L"E00RenderWnd", L"E00", size.x, size.y),
    _system(system) {
}

std::unique_ptr<e00::resource::Bitmap> RenderWindow::convert_to_hardware(const std::unique_ptr<e00::resource::Bitmap> &from) {
  return nullptr;
}

uint8_t RenderWindow::get_pixel(const e00::Vec2<int> &point) const {
  return 0;
}

void RenderWindow::set_pixel(const e00::Vec2<int> &point, uint8_t color) {
}

LRESULT RenderWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return Win32Window::HandleMessage(uMsg, wParam, lParam);
}
