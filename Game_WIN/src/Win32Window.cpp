#include "Win32Window.hpp"

Win32Window::Win32Window(HINSTANCE hInstance, LPCWSTR className, PCWSTR lpWindowName, int width, int height)
  : _width(width),
    _height(height) {
  WNDCLASS wc = { 0 };

  if (!GetClassInfo(hInstance, className, &wc)) {
    wc.lpfnWndProc = &WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);
  }

  _hWnd = CreateWindowEx(0,
    wc.lpszClassName,
    lpWindowName,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    width,
    height,
    nullptr,
    nullptr,
    hInstance,
    /*this*/ nullptr);
  if (_hWnd) {
    SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    // Create an off-screen DC for double-buffering
    auto mainDc = GetDC(_hWnd);
    _hOffScreenDC = CreateCompatibleDC(mainDc);
    _hBitmapMem = CreateCompatibleBitmap(_hOffScreenDC, width, height);
    _hbmOldBackBM = (HBITMAP)SelectObject(_hOffScreenDC, _hBitmapMem);
    ReleaseDC(_hWnd, mainDc);
  }
}

Win32Window::~Win32Window() {
  if (_hWnd) {
    SelectObject(_hOffScreenDC, _hbmOldBackBM);
    DeleteObject(_hBitmapMem);
    DeleteDC(_hOffScreenDC);
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
    return impl->HandleMessageInternal(uMsg, wParam, lParam);
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
LRESULT Win32Window::HandleMessageInternal(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_ERASEBKGND:
      return 1;

    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(_hWnd, &ps);
        // Transfer the off-screen DC to the screen
        BitBlt(hdc, 0, 0, width(), height(), _hOffScreenDC, 0, 0, SRCCOPY);
        EndPaint(_hWnd, &ps);
        return 0;
      }
  }

  return HandleMessage(uMsg, wParam, lParam);
}
void Win32Window::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  ::SetPixel(_hOffScreenDC, x, y, RGB(r, g, b));
  InvalidateRect(_hWnd, nullptr, FALSE);
}
