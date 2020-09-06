#pragma once

#include <array>
#include <chrono>
#include <unordered_map>
#include <string>
#include <system_error>
#include <memory>

#include <Engine/System/OutputScreen.hpp>
#include <Engine/System/InputSystem.hpp>
#include <Engine/System/RootStreamFactory.hpp>
#include <Engine/Stream/StreamFactory.hpp>

#include <Engine/ResourceSystem/Manager.hpp>

#include <Logger/Logger.hpp>
#include <Logger/LoggerSink.hpp>

namespace e00 {
namespace impl {
  class State;
  class ScriptEngine;
}// namespace impl

/**
 * Core class
 */
class Engine {
  friend class impl::State;
  const static std::chrono::milliseconds UPDATE_PERIOD;

  impl::Logger _logger;

  sys::RootStreamFactory *_root_stream_factory;
  std::array<sys::InputSystem *, 4> _input_systems;
  std::unique_ptr<resource::Manager> _resource_manager;
  sys::OutputScreen *_output_screen;
  std::chrono::milliseconds _lag;
  std::chrono::milliseconds _last_render;
  std::unique_ptr<impl::State> _current_state;
  std::unique_ptr<impl::ScriptEngine> _script_engine;

  // Processes a single update
  void tick_update(std::chrono::milliseconds delta);

  // Accumulates input commands to be sent to the current state
  void process_input(sys::eventdata::KeyboardEventData event);
  void process_input(sys::eventdata::MouseEventData event);
  void process_input(sys::eventdata::JoystickEventData event);

  // Values read from configuration ([setup] section)
  std::unordered_map<std::string, std::string> _setup;

  explicit Engine(sys::RootStreamFactory *root_stream_factory);

public:
  static void add_logger_sink(sys::LoggerSink *logger_sink);

  /**
   * Creates a new instance of the engine
   *
   * The owner is responsible to keep the root_stream_factory alive for the entire
   * lifetime of the returned engine instance.
   *
   * @param root_stream_factory the stream factory to use to load paks and resources
   * @return the engine instance or nullptr if an error occurred
   */
  static std::unique_ptr<Engine> Create(sys::RootStreamFactory *root_stream_factory);

  Engine(const Engine &) = delete;

  Engine(Engine &&) noexcept = delete;

  ~Engine();

  Engine &operator=(const Engine &rhs) = delete;

  Engine &operator=(const Engine &&) = delete;

  /**
   * Fetches a value from the [setup] section of the main configuration file
   *
   * @param config_name the configuration name
   * @param default_value default value if config is not found
   * @return the value of the config or `default_value` if it's not found
   */
  [[nodiscard]] std::string_view get_configuration(const std::string &config_name, std::string_view default_value = "") const {
    const auto it = _setup.find(config_name);
    if (it != _setup.end()) {
      return it->second;
    }
    return default_value;
  }

  /**
   * Removes a configuration from memory. Called to save memory
   *
   * @param config_name the configuration to forget
   * @return true if the configuration key was removed, false otherwise
   */
  bool forget_configuration(const std::string &config_name) {
    const auto it = _setup.find(config_name);
    if (it != _setup.end()) {
      _setup.erase(it);
      return true;
    }
    return false;
  }

  /**
   * Forget all configuration keys. Called to save memory
   */
  void clear_configuration() {
    _setup.clear();
  }

  /**
   * Sets the render screen
   * @param output_screen the screen to render to
   */
  void set_output_screen(sys::OutputScreen *output_screen) { _output_screen = output_screen; }

  /**
   * Gets the current render screen
   * @return the current render screen, if set (nullptr otherwise)
   */
  [[nodiscard]] sys::OutputScreen *get_output_screen() const { return _output_screen; }

  /**
   * Tells this instance that shutdown was requested
   */
  void ask_quit();

  /**
   * Adds a resource pack
   *
   * @param pack_name the name of the new pack; this will be loaded by the root stream factory
   * @return error code, if any
   */
  std::error_code add_resource_pack(const std::string& pack_name);

  /**
   * An input system that needs to be queried when updating
   *
   * @param input_system the input system to add
   * @return true if the input system was added successfully
   */
  std::error_code add_input_system(sys::InputSystem *input_system);

  /**
   * Engine is currently running and needs to be updated
   *
   * @return true if the engine is valid and can continue to accept updates
   */
  [[nodiscard]] bool running() const;

  /**
   * Updates physics, timers, processes input systems...
   *
   * @param delta_since_last_call time elapsed since last call to this method
   */
  void update(std::chrono::milliseconds delta_since_last_call);

  /**
   * Draw the current frame to the current output screen
   */
  void render();

  /**
   * Forces a map to start
   *
   * @param map_name the map name to load
   */
  void play_map(const std::string &map_name);
};

}// namespace e00
