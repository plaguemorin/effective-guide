#include "State.hpp"
#include <Engine.hpp>
#include "../ResourceManager.hpp"

namespace e00::impl {
class InitialState : public State {
public:
  InitialState() : State() {}

  ~InitialState() override = default;

  State *update(const std::chrono::milliseconds &) override {
    return nullptr;
  }
};

class ExitState : public State {
public:
  ExitState() : State() {}

  ~ExitState() override = default;

  [[nodiscard]] bool running() const override {
    return false;
  }

  State *update(const std::chrono::milliseconds &) override {
    return nullptr;
  }
};

State *CreateInitialState() {
  return new InitialState();
}

State *CreateQuitState() {
  return new ExitState();
}

const std::unique_ptr<impl::ResourceManager> &State::resource_manager() {
  return _engine->_resource_manager;
}
}// namespace e00::impl
