#pragma once

#include <array>
#include <cstdint>

#include <Engine/CommonTypes.hpp>
#include <Engine/Math.hpp>
#include <Engine/TypedObject.hpp>
#include <Engine/NamedObject.hpp>
#include <Engine/ThingSystem/ComponentContainer.hpp>
#include <Engine/Resource/Resource.hpp>

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
  Vec2I _position;
  int _view_distance;
  int _fov;
  Facing _facing;
  resource_id_t _sprite_id;

protected:
  explicit Thing(type_t type);

public:
  Thing();

  virtual ~Thing();

  void set_position(Vec2I other) {
    _position = other;
  }

  void set_position(Vec2I::type x, Vec2I::type y) {
    _position.x = x;
    _position.y = y;
  }

  [[nodiscard]] const Vec2I& position() const { return _position; }

};
}
