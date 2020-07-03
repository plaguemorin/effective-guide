#include "SystemImpl.hpp"
#include "ResourceStreamSystemImpl.hpp"

#ifdef HAS_POSIX_INPUT

#include "PosixInput.hpp"

#endif

SystemImpl::SystemImpl()
        : _need_quit(false),
          _posix_input(nullptr) {
  _started_time = std::chrono::steady_clock::now();

#ifdef HAS_POSIX_INPUT
  _posix_input = new POSIX_Input();
#endif
}

SystemImpl::~SystemImpl() {
  delete _posix_input;
}

void SystemImpl::yield() {
  // Just in case
  if (time_since_start() >= std::chrono::seconds(10)) {
    _need_quit = true;
  }
}

std::chrono::milliseconds SystemImpl::time_since_start() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - _started_time
  );
}

std::unique_ptr<ResourceStream> SystemImpl::load_file(const std::string& fileName) {
  return std::make_unique<ResourceStreamSystemImpl>(fileName);
}

bool SystemImpl::has_errors() {
  return _need_quit;
}

sys::InputEvent SystemImpl::next_event() {
  if (!_input_queue.empty()) {
    auto el = _input_queue.front();
    _input_queue.pop();
    return el;
  }

  return sys::InputEvent();
}

void SystemImpl::clear_input() {
  while (!_input_queue.empty())
    _input_queue.pop();
}

void SystemImpl::process_input() {
#ifdef HAS_POSIX_INPUT
  while (!_simulated_input_queue.empty()) {
    _input_queue.emplace(_simulated_input_queue.front());
    _simulated_input_queue.pop();
  }

  while (_posix_input && _posix_input->hasInput()) {
    unsigned short key = _posix_input->getKey();
    if (key == 0) // Last line of defense
      break;

    if (key == 0x03) {
      _need_quit = true;
      return;
    }

    _input_queue.emplace(sys::eventdata::KeyboardEventData(
            sys::eventdata::detail::ButtonEventData::Type::PRESSED,
            key
    ));
    _simulated_input_queue.emplace(sys::eventdata::KeyboardEventData(
            sys::eventdata::detail::ButtonEventData::Type::RELEASED,
            key
    ));
  }
#endif
}

