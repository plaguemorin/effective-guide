#pragma once

#include <array>
#include <cstdint>

#include <Engine/TypedObject.hpp>
#include <Engine/NamedObject.hpp>

#include <Engine/CommonTypes.hpp>
#include <Engine/Math.hpp>
#include <Engine/Resource.hpp>
#include <Engine/ThingSystem/ComponentContainer.hpp>

/*
 * While a component system would be nice,
 * don't forget we're targeting slow computers.
 * Unfortunately that means hard-coding stuff.
 */
namespace e00::thing {
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
        public ComponentContainer,
        public NamedObject {
  Vec2<int> _position;
  int _view_distance;
  int _fov;
  Facing _facing;
  resource::id_t _sprite_id;

protected:
  explicit Thing(type_t type);

public:
  Thing();

  virtual ~Thing();

  void set_position(const Vec2I& other) {
    _position = other;
  }

  void set_position(Vec2I::value_type x, Vec2I::value_type y) {
    _position.x() = x;
    _position.y() = y;
  }

  [[nodiscard]] const Vec2I& position() const { return _position; }

};
}
