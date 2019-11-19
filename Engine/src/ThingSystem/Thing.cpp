#include "Engine/ThingSystem/Thing.hpp"

#include <cstring>

namespace thing {
Thing::Thing() : Thing(type_id<Thing>()) {

}

Thing::Thing(type_t type)
        : TypedObject(type),
          ComponentContainer() {
  set_name("THING");
}

Thing::~Thing() {
}

}
