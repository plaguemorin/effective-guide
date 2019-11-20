#pragma once

#include <chrono>

class System {
protected:
  System() = default;

public:
  virtual void yield() const = 0;

  virtual std::chrono::milliseconds timeSinceStart() const = 0;
};
