#include <windows.h>
#include <memory>
#include <Engine.hpp>

#include "Win32System.hpp"

std::unique_ptr<Win32System> _system;

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
  _system = std::make_unique<Win32System>(hInstance);
  _system->Init(nCmdShow);

  Engine e(_system.get());

  // Run the message loop.
  while (e.running()) {
    _system->processWin32();
    e.update();

    // Render
    e.render();
  }

  return 0;
}

