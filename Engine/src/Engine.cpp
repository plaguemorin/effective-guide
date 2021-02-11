#include "Engine.hpp"

#include <Engine/Configuration/Parser.hpp>
#include <Engine/ScriptSystem/ScriptEngine.hpp>
#include <Engine/Resources/Map.hpp>

#include "Error.hpp"
#include "EngineConfiguration.hpp"
#include "MapRender.hpp"

namespace e00 {
// 35Hz is a 28ms period
const std::chrono::milliseconds Engine::UPDATE_PERIOD = std::chrono::milliseconds(28);
constexpr auto ENGINE_CONFIGURATION_FILE_NAME = "GAME.INI";
constexpr auto PACK_CONFIGURATION_FILE_NAME = "pack.ini";

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
  std::unique_ptr<Engine> engine(new Engine());

  // Move the [setup] configs to the engine's so it could be read by the end game
  std::swap(engine->_setup, configuration_listener._setup);

  // Load default packs that are defined in the configuration file...
  for (const auto &default_pack : configuration_listener._packs) {
    if (auto pack = root_stream_factory->load_pack(default_pack)) {
      if (auto pak_ec = engine->add_resource_pack(std::move(pack))) {
        logger.error(SourceLocation(), "Error occurred while adding default pak {}: {}", default_pack, pak_ec.message());
        return nullptr;
      }
    }
  }

  // Add default loaders

  // Initialize things that need resources to validate


  return engine;
}

Engine::Engine()
  : _bad_resource(nullptr),
    _logger("Engine"),
    _input_systems{ { nullptr, nullptr, nullptr, nullptr } },
    _output_screen(nullptr),
    _lag(0),
    _last_render(0),
    _script_engine(impl::ScriptEngine::Create()),
    _running(true) {
  impl::logger_initialize(&_logger);

  _logger.info(SourceLocation(), "Engine configured. Using script engine: {}", _script_engine->engine_name());

  _script_engine->register_function("quit", &Engine::ask_quit);
  _script_engine->register_function("play_map", &Engine::play_map);
  _script_engine->register_function("is_running", &Engine::running);
  //_script_engine->register_function("show_message", &Engine::show_message);
  _script_engine->register_variable("engine", this);
}

Engine::~Engine() {
  _script_engine.reset();
}

bool Engine::running() const {
  return _running;
}

std::error_code Engine::add_resource_pack(std::unique_ptr<sys::StreamFactory> &&pack) {
  // TODO: Clean up this method

  // Sanity check... we don't except to call this method during a critical path
  if (!pack) {
    _logger.error(SourceLocation(), "Passed in an empty pack");
    return make_error_code(EngineErrorCode::invalid_argument);
  }

  // Make sure we have a pack configuration file
  if (auto config_stream = pack->open_stream(PACK_CONFIGURATION_FILE_NAME)) {
    // Parse it to know what we need to index and make available
    class PackConfigParser : public cfg::ParserListener {
      Engine::Pack &pack;
      bool has_basic_info_name;

    public:
      struct ResourceInfo {
        type_t type;
      };

      std::string name;
      std::string script;
      std::map<std::string, ResourceInfo> resources;

      explicit PackConfigParser(Pack &pack)
        : pack(pack),
          has_basic_info_name(false) {}

      std::error_code handle_entry(const cfg::Entry &configuration_entry) override {
        // Empty section is the root
        if (configuration_entry.section.empty()) {
          if (configuration_entry.name == "Script") {
            script = configuration_entry.value;
          } else if (configuration_entry.name == "Name") {
            name = configuration_entry.value;
            has_basic_info_name = true;
          }
        } else {
          // Don't bother to parse if we don't have the minimum basic requirements
          if (!has_basic_info_name) {
            return make_error_code(EngineErrorCode::invalid_pack_configuration);
          }

          // The section is the resource name
          auto &info = resources[std::string(configuration_entry.section)];
          if (configuration_entry.name == "Type") {
            if (configuration_entry.value == "Tilemap") {
              info.type = type_id<e00::resource::Tileset>();
            }
          }
        }

        return {};
      }
    };

    // Create the pack index
    auto &pack_index = _packs.emplace_back();
    pack_index.stream_factory = std::move(pack);

    // Parse the config
    PackConfigParser listener(pack_index);
    if (auto parse_error = cfg::parse(config_stream, &listener)) {
      _logger.error(SourceLocation(), "Failed to parse {} in pack... aborting", PACK_CONFIGURATION_FILE_NAME);
      return parse_error;
    }

    // Ensure the pack has basic info
    if (listener.name.empty()) {
      _logger.error(SourceLocation(), "Pack as no \"Name\" field");
      return make_error_code(EngineErrorCode::invalid_pack_configuration);
    }

    // Load the script to in the script engine if we have one
    if (!listener.script.empty()) {
      _logger.info(SourceLocation(), "Pack {} has a script to parse: {}", listener.name, listener.script);

      if (auto script_stream = pack_index.stream_factory->open_stream(listener.script)) {
        if (auto script_error = _script_engine->parse(script_stream)) {
          _logger.error(SourceLocation(), "Failed to load script {} from pack {}: {}", listener.script, listener.name, script_error.message());
          return script_error;
        }
      } else {
        _logger.error(SourceLocation(), "Failed to load script {} from pack {}. Check {}", listener.script, listener.name, PACK_CONFIGURATION_FILE_NAME);
        return make_error_code(EngineErrorCode::invalid_pack_configuration);
      }
    }

    // Warm up the cache
    for (const auto &resource : listener.resources) {
      _logger.info(SourceLocation(), "Known resource: {} is a {}", resource.first, resource.second.type);
      _resources.emplace_back(resource::make_lazy_ptr<resource::Resource>(resource.first, resource.second.type, this));
    }
  } else {
    // This is a required file so we cannot continue without it
    _logger.error(SourceLocation(), "Missing {} in pack; cannot continue", PACK_CONFIGURATION_FILE_NAME);
    return make_error_code(EngineErrorCode::invalid_pack);
  }

  return {};
}

void Engine::ask_quit() {
  _logger.info(SourceLocation(), "Shutting down");
  _running = false;
}

void Engine::update(std::chrono::milliseconds delta_since_last_call) {
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
  /*  while (_lag >= UPDATE_PERIOD) {
    if (_current_state->needs_steady_updates()) {
      // Update the state
      tick_update(UPDATE_PERIOD);
      _lag -= UPDATE_PERIOD;
    } else {
      tick_update(_lag);
      _lag = std::chrono::milliseconds(0);
    }
  }*/
}

void Engine::process_input(sys::eventdata::KeyboardEventData event) {
  if (event.type == sys::eventdata::KeyboardEventData::RELEASED
      && event.key == sys::eventdata::KeyboardEventData::KEY_ESC) {
    _logger.info(SourceLocation(), "ESC pressed; exiting");
    ask_quit();
    return;
  }

  _logger.info(SourceLocation(), "Got a keyboard event of contained_type {} for key {}", event.type, event.key);

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
  /*  if (auto *new_state = _current_state->update(delta)) {
    _logger.info(SourceLocation(), "Got a new state");
    // We got pushed a new state !!
    _current_state.reset(new_state);
    _current_state->set_engine(this);
  }*/
}

void Engine::render() {
  /*  if (_current_state) {
    _current_state->render();
  }*/
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

  if (auto map = find_resource_t<resource::Map>(map_name)) {
    if (auto tileset = find_resource_t<resource::Tileset>(map->tileset_name())) {
      MapRender render(tileset, map);

      render.render(_output_screen);
    }
  }
}

void Engine::add_resource(std::unique_ptr<resource::Resource> &&resource) {
  if (!resource || find_resource(resource->name()) != _bad_resource) {
    return;
  }

  _resources.emplace_back(std::move(resource), this);
}

const resource::ResourcePtr<resource::Resource> &Engine::find_resource(const std::string_view &name) const {
  for (auto &resource : _resources) {
    if (resource.name() == name) {
      return resource;
    }
  }

  _logger.error(SourceLocation(), "Resource named {} was not found", name);
  return _bad_resource;
}

resource::Resource* Engine::load_resource(resource::detail::ControlBlockObject &source) {
  _logger.info(SourceLocation(), "Loading resource {} as type {}", source.name(), source.contained_type());

  return nullptr;
}

namespace resource::detail {
  void ControlBlockObject::_perform_lazy_load() {
    resource::Resource* ptr = _control_block->loader()->load_resource(*this);
    if (ptr != nullptr) {
      setPtr(ptr);
    }
  }
}// namespace resource::detail
}// namespace e00
