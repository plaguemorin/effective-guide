#pragma once

#include <array>
#include <cstdint>

#include <Engine/Math.hpp>
#include <Engine/TypedObject.hpp>
#include <Engine/NamedObject.hpp>
#include <Engine/Resource/Sprite.hpp>
#include <Engine/ThingSystem/ComponentContainer.hpp>

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
        public ComponentContainer,
        public NamedObject {
  PointI _position;
  Facing _facing;
  resource::resource_id_t _sprite_id;

protected:
  explicit Thing(type_t type);

public:
  Thing();

  ~Thing() override;

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
