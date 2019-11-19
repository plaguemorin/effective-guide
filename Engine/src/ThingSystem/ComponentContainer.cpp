#include "Engine/ThingSystem/ComponentContainer.hpp"

namespace thing {
ComponentContainer::ComponentContainer() {
  for (auto& i : _components) {
    i = nullptr;
  }
}

ComponentContainer::~ComponentContainer() {
  for (auto& i : _components) {
    delete i;
  }
}

Component *ComponentContainer::get_component(const thing_type_t& id) const {
  for (auto& i : _components) {
    if (i && i->type() == id)
      return i;
  }
  return nullptr;
}

bool ComponentContainer::add_component(Component *cmp) {
  for (auto& i : _components) {
    if (i == nullptr) {
      i = cmp;
      return true;
    }
  }
  return false;
}


}
