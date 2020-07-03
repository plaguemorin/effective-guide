#pragma once

#include <windows.h>

#include "Win32Window.hpp"

class Win32System : public Win32Window {
  HINSTANCE _hInstance;
  bool _need_exit;

  std::chrono::steady_clock::time_point _started_time;
public:
  explicit Win32System(HINSTANCE hInstance);

  ~Win32System() override;

  bool Init(int nCmdShow);

  void processWin32();

  bool has_errors() override;

  [[nodiscard]] std::chrono::milliseconds time_since_start() const override;

  std::unique_ptr<ResourceStream> load_file(const std::string& fileName) override;

  sys::InputEvent next_event() override;

  std::unique_ptr<sys::OutputScreen> screen() override;

protected:
  PCWSTR ClassName() const override;

  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};


