#include "Win32Window.hpp"

Win32Window::Win32Window(HINSTANCE hInstance, LPCWSTR className, PCWSTR lpWindowName, int width, int height) {
  WNDCLASS wc = { 0 };

  if (!GetClassInfo(hInstance, className, &wc)) {
    wc.lpfnWndProc = &WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);
  }

  _hWnd = CreateWindowEx(0, wc.lpszClassName, lpWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, /*this*/nullptr);
  SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);
}

Win32Window::~Win32Window() {
  if (_hWnd) {
    DestroyWindow(_hWnd);
    _hWnd = nullptr;
  }
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  Win32Window *impl = nullptr;

  if (uMsg == WM_NCCREATE) {
    if (auto *pCreate = (CREATESTRUCT *)lParam) {
      if ((impl = (Win32Window *)pCreate->lpCreateParams)) {
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)impl);
      }
    }
  } else {
    impl = (Win32Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  }

  if (impl) {
    return impl->HandleMessage(uMsg, wParam, lParam);
  } else {
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

LRESULT Win32Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}

void Win32Window::show() {
  ShowWindow(_hWnd, SW_SHOW);
}

void Win32Window::show(int showCmd) {
  ShowWindow(_hWnd, showCmd);
}
