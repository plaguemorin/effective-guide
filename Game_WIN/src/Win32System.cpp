#include <string>
#include <iostream>

#include "Win32System.hpp"
#include "Win32FileStream.hpp"


Win32System::Win32System(HINSTANCE hInstance)
  : _hInstance(hInstance),
    _need_exit(false) {
  AllocConsole();

  ::freopen_s(&_newStdout, "CONOUT$", "w", stdout);
  ::freopen_s(&_newStderr, "CONOUT$", "w", stderr);
  ::freopen_s(&_newStdin, "CONIN$", "r", stdin);

  // Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
  // to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
  // which seems to occur in newer version of Visual Studio even when the console has not been read from or written
  // to yet.
  std::cout.clear();
  std::cerr.clear();
  std::cin.clear();

  std::wcout.clear();
  std::wcerr.clear();
  std::wcin.clear();
}

Win32System::~Win32System() {
}

bool Win32System::Init(int nCmdShow) {
  if (Create(L"E00", WS_OVERLAPPEDWINDOW)) {
    ShowWindow(windowHandle(), nCmdShow);
  }
  return false;
}

void Win32System::processWin32() {
  MSG msg = {};

  while (PeekMessage(&msg, windowHandle(), 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

PCWSTR Win32System::ClassName() const {
  return L"E00CLZ";
}

LRESULT Win32System::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      _need_exit = true;
      return 0;
  }

  return DefWindowProc(windowHandle(), uMsg, wParam, lParam);
}
