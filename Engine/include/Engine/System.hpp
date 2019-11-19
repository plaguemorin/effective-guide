#pragma once

#include <string>
#include <chrono>

class System {
public:
  System() = default;

  virtual ~System() = default;

  virtual void yield() const = 0;

  virtual std::chrono::milliseconds timeSinceStart() const = 0;
};
