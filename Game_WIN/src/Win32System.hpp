#pragma once

#include <windows.h>
#include <chrono>
#include <memory>

#include "Win32Window.hpp"

#include <Engine/Stream/StreamFactory.hpp>

class Win32System {
  HINSTANCE _hInstance;
  bool _need_exit;

  FILE *_newStdout = nullptr;
  FILE *_newStderr = nullptr;
  FILE *_newStdin = nullptr;

  Win32Window *_window;

public:
  explicit Win32System(HINSTANCE hInstance);

  ~Win32System();

  void processWin32();

  HINSTANCE instance() const { return _hInstance; }
  bool needsQuit();
};
