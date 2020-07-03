#pragma once

#include <queue>

class POSIX_Input;

class SystemImpl  {
  std::chrono::steady_clock::time_point _started_time;
  bool _need_quit;
  std::queue<sys::InputEvent> _input_queue;
  std::queue<sys::InputEvent> _simulated_input_queue;

  POSIX_Input *_posix_input;

public:
  SystemImpl();

  ~SystemImpl() override;

  void yield() override;

  [[nodiscard]] std::chrono::milliseconds time_since_start() const override;

  std::unique_ptr<ResourceStream> load_file(const std::string& fileName) override;

  bool has_errors() override;

  sys::InputEvent next_event() override;

  void clear_input();

  void process_input();

};


