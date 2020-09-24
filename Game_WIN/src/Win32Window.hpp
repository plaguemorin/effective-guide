#pragma once

#include <windows.h>

class Win32Window {
  HWND _hWnd;

protected:
  Win32Window(HINSTANCE hInstance, LPCWSTR className, PCWSTR lpWindowName, int width, int height);

  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  virtual ~Win32Window();

  void show();

  void show(int showCmd);
};
