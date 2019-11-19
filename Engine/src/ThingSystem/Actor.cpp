#include "Engine/ThingSystem/Actor.hpp"

namespace thing {
Actor::Actor() : Actor(type_id<Actor>()) {}

Actor::Actor(thing_type_t type) : Thing(type) {

}

Actor::~Actor() {}
}
