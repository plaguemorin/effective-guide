#include "Win32Window.hpp"

Win32Window::Win32Window() : _hwnd(nullptr) {

}

Win32Window::~Win32Window() {
  if (_hwnd) {
    DestroyWindow(_hwnd);
    _hwnd = nullptr;
  }
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  Win32Window *impl = nullptr;

  if (uMsg == WM_NCCREATE) {
    auto *pCreate = (CREATESTRUCT *) lParam;
    impl = (Win32Window *) pCreate->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) impl);

    impl->_hwnd = hwnd;
  } else {
    impl = (Win32Window *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
  }

  if (impl) {
    return impl->HandleMessage(uMsg, wParam, lParam);
  } else {
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

bool Win32Window::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu) {
  WNDCLASS wc = {0};
  wc.lpfnWndProc = &WindowProc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.lpszClassName = ClassName();
  RegisterClass(&wc);

  _hwnd = CreateWindowEx(
          dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
          nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(nullptr), this
  );
  return _hwnd != nullptr;
}
