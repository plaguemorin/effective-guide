#include <cstdio>
#include <crt0.h>
#include <sys/nearptr.h>
#include <dpmi.h>
#include <csignal>

#include "Engine.hpp"
#include "DosDetect.hpp"
#include "DosMode13.hpp"
#include "MouseInt33/MouseComDrv.hpp"
#include "StdFile.hpp"
#include "KeyboardDecoding/KeyboardEventHandler.hpp"

#include "Signals.hpp"
#include "Timer.hpp"

int _crt0_startup_flags = _CRT0_FLAG_LOCK_MEMORY | _CRT0_FLAG_NONMOVE_SBRK;

namespace {
void signal_handler(int sig) {
  raise(sig);
}

class DosFS : public e00::sys::RootStreamFactory {
  const std::string _base_directory;

public:
  explicit DosFS(std::string base_directory) : _base_directory(std::move(base_directory)) {}

  DosFS() : _base_directory("RES/") {}

  ~DosFS() override = default;

  std::unique_ptr<e00::sys::StreamFactory> load_pack(const std::string &pack_name) override {
    return std::unique_ptr<DosFS>(new DosFS(_base_directory + pack_name + ".pak/"));
  }

  std::unique_ptr<e00::Stream> open_stream(const std::string &fileName) override {
    return StdFile::CreateFromFilename(_base_directory + fileName);
  }
};

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

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  if (__djgpp_nearptr_enable() == 0) {
    std::puts("Failed to disable memory protection for direct VGA access");
    return -1;
  }

  // Early Init
  LoggerSink logger_sink;
  e00::Engine::add_logger_sink(&logger_sink);

  std::puts("Initialize System: ");
  SystemInfo info = guessSystemInfo();
  switch (info.type) {
    case SystemType::UNSUPPORTED: break;
    case SystemType::DOS: std::puts("DOS"); break;
    case SystemType::WINDOWS_3X: std::puts("Windows 3.X"); break;
    case SystemType::WINDOWS_95: std::puts("Windows 95"); break;
    case SystemType::WINDOWS_98: std::puts("Windows 98"); break;
    case SystemType::WINDOWS_ME: std::puts("Windows ME"); break;
    case SystemType::WINDOWS_NT: std::puts("Windows NT"); break;
    case SystemType::STEALTH_WINDOWS: std::puts("Windows (Stealth)"); break;
    case SystemType::OS2_WARP: std::puts("OS/2 Warp"); break;
    case SystemType::OS2: std::puts("OS/2"); break;
    case SystemType::DOSEMU: std::puts("DOSEmu"); break;
    case SystemType::OPENDOS: std::puts("OpenDOS"); break;
    case SystemType::OPENDOS_EMM386: std::puts("OpenDOS EMM386"); break;
  }

  signals_init(&signal_handler);
  std::puts("Initialize Signals: Done");

  // Video init
  const auto video_info = video_get_info();
  if (video_info.ega_info.present) {
    std::puts("Found EGA");
  }
  if (video_info.vga_info.present) {
    std::puts("Found VGA");
  }

//  auto initial_video_mode = video_get_current_mode();

  timer_init();
  std::puts("Initialize Timer Handler: Done");

  // Fire up the engine
  DosFS dos_fs;
  auto engine = e00::Engine::Create(&dos_fs);

  const auto video_requested = engine->get_configuration("video");
  std::puts("Video subsystem requested: ");
  std::puts(video_requested.data());

  std::puts("Press ENTER to continue");
  std::getchar();

  // Last chance before we hook the inputs

  KeyboardEventHandler keyboard_event_handler;
  engine->add_input_system(&keyboard_event_handler);
  std::puts("Initialize Keyboard Handler: Done");

  MouseComDrv mouse_drv;
  if (mouse_drv.has_mouse()) {
    engine->add_input_system(&mouse_drv);
    std::puts("Initialize Mouse: Mouse found");
  } else {
    std::puts("Initialize Mouse: Mouse not found");
  }

  // who cares about anything, set vga 320x200 8bit
/*
  video_set_current_mode(0x13);
  engine.set_output_screen(video_get_screen());
*/

  /*
  DOSMode13 gfx;
  if (!gfx.valid()) {
    printf("Bad video: requires EGA or VGA\n");
    return -1;
  }
  gfx.initialize();
  */

  auto current_time = timer_since_start();
  engine->clear_configuration();

  engine->play_map("Overworld");

  while (engine->running()) {
    const auto tss = timer_since_start();
    mouse_drv.mouse_update();
    __dpmi_yield();
    engine->update(tss - current_time);
//    engine.render();
    current_time = tss;
  }

  delete engine->get_output_screen();
//  video_set_current_mode(initial_video_mode);
  std::puts("Exit");

  timer_shutdown();
  signals_shutdown();

  return 0;
}
