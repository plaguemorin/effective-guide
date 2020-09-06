#include <windows.h>
#include <memory>

#include <Logger/LoggerSink.hpp>
#include <Engine.hpp>

#include "Win32System.hpp"
#include "Win32StreamFactory.hpp"

namespace {
class LoggerSink : public e00::sys::LoggerSink {
public:
  void info(const std::string_view &string) override {
    puts(string.data());
  }

  void error(const std::string_view &string) override {
    fputs(string.data(), stderr);
    fputc('\n', stderr);
  }
};
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*unused*/, PWSTR pCmdLine, int nCmdShow) {
  Win32System system(hInstance);
  system.Init(nCmdShow);

  // Early Init
  LoggerSink logger_sink;
  e00::Engine::add_logger_sink(&logger_sink);

  // Create the engine
  Win32StreamFactory stream_factory("");
  auto e = e00::Engine::Create(&stream_factory);

  if (!e) {
    return 1;
  }

  // Set the output screen
  e->set_output_screen(nullptr);

  e->play_map("Overworld");

  // Run the message loop.
  while (e->running()) {
    system.processWin32();
    e->update(std::chrono::milliseconds(5));

    // Render
    e->render();
  }

  return 0;
}
