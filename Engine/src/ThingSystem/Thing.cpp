#include "Engine/ThingSystem/Thing.hpp"

#include <cstring>

namespace e00::thing {
Thing::Thing() : Thing(type_id<Thing>()) {
}

Thing::Thing(type_t type)
  : TypedObject(type),
    ComponentContainer(),
    NamedObject("THING") {
}

Thing::~Thing() {
}

}// namespace thing
