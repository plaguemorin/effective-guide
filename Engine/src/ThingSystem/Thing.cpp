#include "Engine/ThingSystem/Thing.hpp"

#include <cstring>

namespace thing {
Thing::Thing() : Thing(type_id<Thing>()) {

}

Thing::Thing(thing_type_t type)
        : TypedObject(type),
          ComponentContainer() {
  set_name("THING");
}

Thing::~Thing() {
}

void Thing::set_name(const char *name) {
  strncpy(_name.data(), name, _name.size());
}


}
