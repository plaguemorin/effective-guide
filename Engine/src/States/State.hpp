#pragma once

#include <chrono>
#include <string>
#include <system_error>

#include <Engine/ResourceSystem/Manager.hpp>

namespace e00 {
class Engine;

namespace sys {
  class OutputScreen;
}

namespace impl {
  class State {
  protected:
    Engine *_engine;
    const std::unique_ptr<resource::Manager>& resource_manager();

  public:
    explicit State()
      : _engine(nullptr) {}

    virtual ~State() = default;

    void set_engine(Engine *engine) { _engine = engine; }

    [[nodiscard]] virtual bool needs_steady_updates() const { return false; }

    // TODO: Maybe we need this ?
    //[[nodiscard]] virtual std::chrono::milliseconds steady_update_rate() const { return std::chrono::milliseconds(0); }

    [[nodiscard]] virtual bool running() const { return true; }

    virtual void render() {}

    [[nodiscard]] virtual State *update(const std::chrono::milliseconds &delta_since_last_update) = 0;
  };

  /**
 * Creates a dummy state that immediately transitions into an INTRO state without
 * using the _engine field.
 *
 * @return an initial state
 */
  State *CreateInitialState();

  /**
 * Creates a dummy state that has the running field to false so the engine can
 * exit gracefully
 *
 * @return a simple quit state
 */
  State *CreateQuitState();
}// namespace impl
}// namespace e00
