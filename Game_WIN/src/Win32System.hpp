#pragma once

#include <windows.h>
#include <chrono>
#include <memory>

#include "Win32Window.hpp"

#include <Engine/Stream/StreamFactory.hpp>

class Win32System : public Win32Window {
  HINSTANCE _hInstance;
  bool _need_exit;

  FILE * _newStdout = nullptr;
  FILE * _newStderr = nullptr;
  FILE * _newStdin = nullptr;

public:
  explicit Win32System(HINSTANCE hInstance);

  ~Win32System() override;

  bool Init(int nCmdShow);

  void processWin32();

protected:
  PCWSTR ClassName() const override;

  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};


