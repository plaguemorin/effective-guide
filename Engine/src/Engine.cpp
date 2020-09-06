#include "Engine.hpp"

#include <Engine/Configuration/Parser.hpp>
#include <Engine/ScriptSystem/ScriptEngine.hpp>

#include "States/State.hpp"
#include "States/LoadingMap.hpp"
#include "Error.hpp"
#include "EngineConfiguration.hpp"

namespace e00 {
// 35Hz is a 28ms period
const std::chrono::milliseconds Engine::UPDATE_PERIOD = std::chrono::milliseconds(28);
constexpr auto ENGINE_CONFIGURATION_FILE_NAME = "GAME.INI";

std::unique_ptr<Engine> Engine::Create(sys::RootStreamFactory *root_stream_factory) {
  impl::Logger logger("EngineCreate");

  if (root_stream_factory == nullptr) {
    logger.error(SourceLocation(), "Root stream factory is null; can't continue");
    return nullptr;
  }

  // Read GAME.INI from the "Game" resource directory
  impl::EngineConfiguration configuration_listener;

  // PORTING: IF YOUR PLATFORM NEED FULL PATH (XBOX) PLEASE IMPLEMENT IN THE StreamFactory SUBCLASS
  if (auto configuration_stream = root_stream_factory->open_stream(ENGINE_CONFIGURATION_FILE_NAME)) {
    if (auto error_parsing = cfg::parse(configuration_stream, &configuration_listener)) {
      // Can't read GAME.INI?
      // Exit early, if you get this make sure you read the PORTING note above
      logger.error(SourceLocation(), "Failed to parse main configuration, exiting: {}", error_parsing.message());
      return nullptr;
    }
  } else {
    // Error !
    // Exit early, if you get this make sure you read the PORTING note above
    logger.error(SourceLocation(), "Failed to locate main configuration file: {}, exiting", ENGINE_CONFIGURATION_FILE_NAME);
    return nullptr;
  }

  // Should we enable debug ?
  if (configuration_listener._enable_diagnostics) {
    //impl::Logger::set_log_level(impl::Logger::debug);
  }

  // Make sure we have values (we'll check them later, but we need them to be not empty at least)
  if (configuration_listener._start_map.empty()) {
    logger.error(SourceLocation(), "Starting map cannot be empty, aborting");
    return nullptr;
  }

  // Create the "real engine"
  std::unique_ptr<Engine> engine(new Engine(root_stream_factory));

  // Move the [setup] configs to the engine's so it could be read by the end game
  std::swap(engine->_setup, configuration_listener._setup);

  // Load default packs that are defined in the configuration file...
  for (const auto &default_pack : configuration_listener._packs) {
    if (auto pak_ec = engine->add_resource_pack(default_pack)) {
      logger.error(SourceLocation(), "Error occurred while adding default pak {}: {}", default_pack, pak_ec.message());
      return nullptr;
    }
  }

  // Add default loaders

  // Initialize things that need resources to validate


  return engine;
}

Engine::Engine(sys::RootStreamFactory *root_stream_factory)
  : _logger("Engine"),
    _root_stream_factory(root_stream_factory),
    _input_systems{ { nullptr, nullptr, nullptr, nullptr } },
    _resource_manager(new resource::Manager()),
    _output_screen(nullptr),
    _lag(0),
    _last_render(0),
    _current_state(impl::CreateInitialState()),
    _script_engine(impl::ScriptEngine::Create()) {
  impl::logger_initialize(&_logger);

  _logger.info(SourceLocation(), "Engine configured. Using script engine: {}", _script_engine->engine_name());

  _script_engine->register_function("running", [&]() { return this->running(); });
}

Engine::~Engine() {
  _script_engine.reset();
  _current_state.reset();
}

bool Engine::running() const {
  if (_current_state == nullptr) {
    return false;
  }

  return _current_state->running();
}

std::error_code Engine::add_resource_pack(const std::string &pack_name) {
  if (auto pak = _root_stream_factory->load_pack(pack_name)) {
    return _resource_manager->add_pack(std::move(pak));
  }

  return e00::make_error_code(EngineErrorCode::no_stream_loader_for_pak);
}

void Engine::ask_quit() {
  _logger.info(SourceLocation(), "Shutting down");
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
    _logger.info(SourceLocation(), "ESC pressed; exiting");
    ask_quit();
    return;
  }

  _logger.info(SourceLocation(), "Got a keyboard event of type {} for key {}", event.type, event.key);

  // Process bindings
}

void Engine::process_input(sys::eventdata::MouseEventData event) {
  (void)event;

  _logger.info(SourceLocation(), "Got mouse event!");
}

void Engine::process_input(sys::eventdata::JoystickEventData event) {
  (void)event;

  _logger.info(SourceLocation(), "Got a joystick event!");
}

void Engine::tick_update(std::chrono::milliseconds delta) {
  if (auto *new_state = _current_state->update(delta)) {
    _logger.info(SourceLocation(), "Got a new state");
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
        _logger.error(SourceLocation(), "Failed to initialize input system", ec.message());
        return make_error_code(EngineErrorCode::failed_to_initialize_input);
      }

      _input_system = input_system;
      return {};
    }
  }

  _logger.error(SourceLocation(), "Failed to add input system: too many input devices");
  return make_error_code(EngineErrorCode::too_many_input_systems);
}

void Engine::add_logger_sink(sys::LoggerSink *logger_sink) {
  impl::logger_add_root_sink(logger_sink);
}

void Engine::play_map(const std::string &map_name) {
  _logger.info(SourceLocation(), "Asked for map named {}", map_name);

  _current_state.reset();
  _current_state = std::make_unique<impl::Loading_Map>(map_name);
  _current_state->set_engine(this);
}

}// namespace e00
