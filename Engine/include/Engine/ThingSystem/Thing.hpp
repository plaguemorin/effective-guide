#pragma once

#include <array>
#include <cstdint>

#include <Engine/Math.hpp>
#include <Engine/ThingSystem/ComponentContainer.hpp>
#include <Engine/ThingSystem/TypedObject.hpp>

/*
 * While a component system would be nice,
 * don't forget we're targeting slow computers.
 * Unfortunately that means hard-coding stuff
 */
namespace thing {
enum class Facing {
  NORTH,
  NORTH_EAST,
  EAST,
  SOUTH_EAST,
  SOUTH,
  SOUTH_WEST,
  WEST,
  NORTH_WEST,
};


/**
 * World object (exists in the world)
 */
class Thing :
        public TypedObject,
        public ComponentContainer {
  std::array<char, 10> _name;
  PointI _position;
  Facing _facing;

protected:
  explicit Thing(thing_type_t type);

public:
  Thing();

  ~Thing() override;

  void set_name(const char *name);

  [[nodiscard]] const char *name() const { return _name.data(); }

  void set_position(PointI other) {
    _position = other;
  }

  void set_position(PointI::type x, PointI::type y) {
    _position.x = x;
    _position.y = y;
  }

  [[nodiscard]] const PointI& position() const { return _position; }

};
}
