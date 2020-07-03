#pragma once

#include <windows.h>

class Win32Window {
protected:
  [[nodiscard]] virtual PCWSTR ClassName() const = 0;

  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

  HWND _hwnd;

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  Win32Window();

  virtual ~Win32Window();

  bool Create(
          PCWSTR lpWindowName,
          DWORD dwStyle,
          DWORD dwExStyle = 0,
          int x = CW_USEDEFAULT,
          int y = CW_USEDEFAULT,
          int nWidth = CW_USEDEFAULT,
          int nHeight = CW_USEDEFAULT,
          HWND hWndParent = nullptr,
          HMENU hMenu = nullptr
  );

  [[nodiscard]] HWND windowHandle() const { return _hwnd; }
};


