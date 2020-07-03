#include "Engine/ThingSystem/ComponentContainer.hpp"

namespace e00::thing {
ComponentContainer::ComponentContainer()  {
  for (auto& i : _components) {
    i = nullptr;
  }
}

ComponentContainer::~ComponentContainer() = default;

Component* ComponentContainer::get_component(const type_t& id) const {
  for (auto& i : _components) {
    if (i && i->type() == id)
      return i;
  }
  return nullptr;
}


}
