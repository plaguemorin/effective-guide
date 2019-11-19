#include "Engine/ThingSystem/Manager.hpp"

namespace thing {

void Manager::Register(thing::Thing *aThing) {
  _things.push_back(aThing);
}

void Manager::Unregister(Thing *aThing) {
  auto it = _things.begin();
  while (it != _things.end()) {
    if (*it == aThing) {
      _things.erase(it);
      return;
    }
  }
}
}
