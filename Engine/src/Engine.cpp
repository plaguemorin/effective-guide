#include "Engine.hpp"

#include "States/State.hpp"
#include "States/LoadingMap.hpp"

#include "ResourceManager.hpp"

#include "EngineConfiguration.hpp"
#include "Logger.hpp"
#include "ResourceConfiguration.hpp"

namespace e00 {

// 35Hz is a 28ms period
const std::chrono::milliseconds Engine::UPDATE_PERIOD = std::chrono::milliseconds(28);

Engine::Engine(sys::ResourceStreamLoader *stream_loader)
  : _logger(new impl::Logger("Engine")),
    _resource_manager(new impl::ResourceManager(stream_loader)),
    _output_screen(nullptr),
    _lag(0),
    _last_render(0),
    _current_state(impl::CreateInitialState()) {

  impl::logger_initialize(_logger);

  // Reset all input system
  for (auto &i : _input_systems) {
    i = nullptr;
  }

  // Read GAME.INI from the "Game" resource directory
  impl::EngineConfiguration configuration_listener;
  if (auto error_parsing = _resource_manager->parse_configuration("GAME.INI", &configuration_listener)) {
    // Can't read GAME.INI?
    // Exit early, if you get this make sure you read the PORTING note above
    _logger->error(SourceLocation(), "Failed to parse GAME.INI, exiting: {}", error_parsing.message());
    _current_state.reset(impl::CreateQuitState());
    return;
  }

  _logger->info(SourceLocation(), "Engine configured");

  // Move the [setup] configs
  std::swap(_setup, configuration_listener._setup);
}

Engine::Engine(Engine &&other) noexcept {
  // TODO
  (void)other;
}

Engine::~Engine() {
  _current_state.reset();
  delete _logger;
}

bool Engine::running() const {
  if (_current_state == nullptr) {
    return false;
  }

  return _current_state->running();
}

std::error_code Engine::add_resource_pack(sys::ResourceStreamLoader *pack_loader) {
  // Proxy the call: this is handled by the resource manager
  return _resource_manager->add_pack(pack_loader);
}

void Engine::ask_quit() {
  _logger->info(SourceLocation(), "Shutting down");
  _current_state.reset(impl::CreateQuitState());
}

void Engine::update(std::chrono::milliseconds delta_since_last_call) {
  // While technically invalid; protect against the state going away
  if (_current_state == nullptr) {
    return;
  }

  // Translate the input events to the current state
  for (const auto &i : _input_systems) {
    if (i == nullptr) {
      continue;
    }

    while (auto event = i->next_event()) {
      switch (event.type) {
        case sys::InputEvent::Type::NONE: continue;
        case sys::InputEvent::Type::KEYBOARD:
          process_input(event.keyboard_data);
          break;
        case sys::InputEvent::Type::MOUSE:
          process_input(event.mouse_data);
          break;
        case sys::InputEvent::Type::JOYSTICK:
          process_input(event.joystick_data);
          break;
      }
    }
  }

  // Compute how much we need to update the logic
  _lag += delta_since_last_call;

  // Run the input logic until we're caught up
  while (_lag >= UPDATE_PERIOD) {
    if (_current_state->needs_steady_updates()) {
      // Update the state
      tick_update(UPDATE_PERIOD);
      _lag -= UPDATE_PERIOD;
    } else {
      tick_update(_lag);
      _lag = std::chrono::milliseconds(0);
    }
  }
}

void Engine::process_input(sys::eventdata::KeyboardEventData event) {
  if (event.type == sys::eventdata::KeyboardEventData::RELEASED
      && event.key == sys::eventdata::KeyboardEventData::KEY_ESC) {
    _logger->info(SourceLocation(), "ESC pressed; exiting");
    ask_quit();
    return;
  }

  _logger->info(SourceLocation(), "Got a keyboard event of type {} for key {}", event.type, event.key);

  // Process bindings
}

void Engine::process_input(sys::eventdata::MouseEventData event) {
  (void)event;
}

void Engine::process_input(sys::eventdata::JoystickEventData event) {
  (void)event;

  _logger->info(SourceLocation(), "Got a joystick event!");
}

void Engine::tick_update(std::chrono::milliseconds delta) {
  if (auto *new_state = _current_state->update(delta)) {
    _logger->info(SourceLocation(), "Got a new state");
    // We got pushed a new state !!
    _current_state.reset(new_state);
    _current_state->set_engine(this);
  }
}

void Engine::render() {
  if (_current_state) {
    _current_state->render();
  }
}

std::error_code Engine::add_input_system(sys::InputSystem *input_system) {
  for (auto &_input_system : _input_systems) {
    if (_input_system == nullptr) {
      if (auto ec = input_system->init()) {
        _logger->error(SourceLocation(), "Failed to initialize input system", ec.message());
        return make_error_code(EngineErrorCode::failed_to_initialize_input);
      }

      _input_system = input_system;
      return {};
    }
  }

  _logger->error(SourceLocation(), "Failed to add input system: too many input devices");
  return make_error_code(EngineErrorCode::too_many_input_systems);
}

void Engine::add_logger_sink(sys::LoggerSink *logger_sink) {
  impl::logger_add_root_sink(logger_sink);
}

}// namespace e00
