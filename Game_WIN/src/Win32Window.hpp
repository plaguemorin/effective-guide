#pragma once

#include <windows.h>
#include <cstdint>

class Win32Window {
  HWND _hWnd;
  int _width, _height;
  HBITMAP _hBitmapMem, _hbmOldBackBM;
  HDC _hOffScreenDC;

  LRESULT HandleMessageInternal(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  Win32Window(HINSTANCE hInstance, LPCWSTR className, PCWSTR lpWindowName, int width, int height);

  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  virtual ~Win32Window();

  void show();

  void show(int showCmd);

  const int width() const { return _width; }

  const int height() const { return _height; }

  void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
};
