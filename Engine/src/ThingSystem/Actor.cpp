#include "Engine/ThingSystem/Actor.hpp"

namespace e00::thing {
Actor::Actor() : Actor(type_id<Actor>()) {}

Actor::Actor(type_t type) : Thing(type) {

}

Actor::~Actor() {}
}
