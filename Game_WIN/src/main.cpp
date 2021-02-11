#include <windows.h>
#include <memory>

#include <Logger/LoggerSink.hpp>
#include <Engine.hpp>

#include <Engine/Resources/Tileset.hpp>
#include <Engine/Resources/Map.hpp>

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

  RenderWindow output_screen(system, { 640, 480 });
  output_screen.show(nCmdShow);

  // Set the output screen
  e->set_output_screen(&output_screen);

  auto map = std::make_unique<e00::resource::Map>("CloudCity", 20, 20);
  map->set_tileset_name("TOTG");
  auto &ground = map->add_static_layer(e00::resource::Map::LayerID::BACKGROUND);
  ground.set_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 0, 0, 92, 362, 363, 363, 363, 363, 363, 363, 363, 363, 363, 363, 363, 363, 363, 363, 364, 94, 0, 0, 92, 407, 368, 368, 368, 368, 368, 368, 368, 368, 368, 368, 413, 368, 368, 368, 409, 94, 0, 0, 92, 407, 368, 368, 369, 368, 368, 371, 368, 368, 368, 414, 413, 368, 368, 368, 409, 94, 0, 0, 92, 407, 368, 368, 368, 368, 368, 368, 368, 368, 372, 368, 368, 413, 369, 368, 409, 94, 0, 0, 189, 452, 453, 410, 368, 368, 368, 368, 368, 368, 368, 368, 368, 368, 368, 368, 409, 94, 0, 0, 234, 235, 188, 452, 410, 368, 414, 368, 368, 369, 368, 372, 368, 368, 413, 368, 409, 94, 0, 0, 0, 182, 233, 190, 407, 368, 413, 413, 413, 368, 368, 368, 368, 368, 368, 368, 409, 94, 0, 0, 0, 0, 53, 144, 407, 368, 371, 368, 413, 368, 368, 368, 368, 368, 372, 368, 409, 94, 0, 0, 53, 145, 98, 93, 407, 368, 368, 368, 413, 368, 371, 413, 413, 368, 368, 368, 409, 94, 0, 0, 143, 242, 243, 244, 407, 368, 368, 368, 368, 368, 368, 368, 413, 413, 368, 368, 409, 94, 0, 0, 92, 287, 288, 289, 407, 368, 369, 368, 413, 368, 413, 368, 368, 413, 368, 413, 409, 94, 0, 0, 92, 332, 333, 334, 407, 368, 368, 368, 414, 368, 368, 368, 368, 414, 368, 368, 409, 94, 0, 0, 92, 362, 363, 363, 365, 368, 413, 368, 413, 368, 371, 368, 368, 368, 368, 368, 409, 94, 0, 0, 92, 407, 368, 414, 368, 368, 368, 368, 368, 368, 368, 368, 368, 372, 368, 411, 454, 192, 0, 0, 189, 452, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453, 453, 454, 191, 237, 0, 0, 234, 235, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 138, 236, 237, 0, 0, 0, 0, 182, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 184, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  e->add_resource(std::move(map));
  e->play_map("CloudCity");

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
