#include <windows.h>
#include <memory>

#include <Logger/LoggerSink.hpp>
#include <Engine.hpp>

#include "Win32System.hpp"
#include "Win32StreamFactory.hpp"
#include "RenderWindow.hpp"

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
}// namespace

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*unused*/, PWSTR pCmdLine, int nCmdShow) {
  Win32System system(hInstance);

  // Early Init
  LoggerSink logger_sink;
  e00::Engine::add_logger_sink(&logger_sink);

  // Create the engine
  Win32StreamFactory stream_factory("");
  auto e = e00::Engine::Create(&stream_factory);

  if (!e) {
    logger_sink.error("Failed to initialize engine");
    return 1;
  }

  RenderWindow output_screen(system, {640, 480});
  output_screen.show(nCmdShow);

  // Set the output screen
  e->set_output_screen(&output_screen);

  // Run the message loop.
  while (e->running()) {
    system.processWin32();
    if (system.needsQuit()) {
      e->ask_quit();
      continue;
    }
    e->update(std::chrono::milliseconds(5));
    e->render();
  }

  return 0;
}
