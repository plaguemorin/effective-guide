#include "Win32System.hpp"
#include "Win32FileStream.hpp"

namespace {
std::wstring s2ws(const std::string& s) {
  auto str_length = (int) s.length() + 1;
  auto len = MultiByteToWideChar(
          CP_ACP,
          0,
          s.c_str(),
          str_length,
          nullptr,
          0
  );

  std::wstring r;
  r.resize(len);

  MultiByteToWideChar(
          CP_ACP,
          0,
          s.c_str(),
          str_length,
          r.data(),
          r.length()
  );

  return r;
}
}

Win32System::Win32System(HINSTANCE hInstance) : _hInstance(hInstance), _need_exit(false) {
  _started_time = std::chrono::steady_clock::now();
}

Win32System::~Win32System() {

}

bool Win32System::Init(int nCmdShow) {
  if (Create(L"E00", WS_OVERLAPPEDWINDOW)) {
    ShowWindow(windowHandle(), nCmdShow);
  }
  return false;
}

bool Win32System::has_errors() {
  return _need_exit;
}

std::chrono::milliseconds Win32System::time_since_start() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - _started_time
  );
}

std::unique_ptr<ResourceStream> Win32System::load_file(const std::string& fileName) {
  // Load up file from the resource folder
  auto wstr = s2ws(fileName);
  auto f = CreateFile(
          wstr.c_str(),
          GENERIC_READ,
          0,
          nullptr,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          nullptr
  );

  if (f == INVALID_HANDLE_VALUE) {
    // Well, screw this!
    return nullptr;
  }

  return std::make_unique<Win32FileStream>(f);
}

sys::InputEvent Win32System::next_event() {
  return sys::InputEvent();
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

std::unique_ptr<sys::OutputScreen> Win32System::screen() {
  return nullptr;
}
