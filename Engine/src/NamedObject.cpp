#include "Engine/NamedObject.hpp"

#include <cstring>

NamedObject::NamedObject() : _name{} {

}

NamedObject::NamedObject(const NamedObject& other) {
  set_name(other.name());
}

NamedObject::~NamedObject() {

}

void NamedObject::set_name(const char *name) {
  strncpy(_name.data(), name, _name.size());
}
