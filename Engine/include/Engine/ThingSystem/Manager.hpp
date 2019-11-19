#pragma once

#include <vector>
#include <Engine/ThingSystem/Thing.hpp>

namespace thing {
class Manager {
  std::vector<Thing *> _things;

public:
  Manager() = default;

  ~Manager() = default;

  void Register(Thing *aThing);

  void Unregister(Thing *aThing);

  template<typename T, typename Fnc, typename...Args>
  void WithAllThingsOfType(Fnc func, Args&& ... args) const {
    for (const auto& thing : _things) {
      if (thing->type() == type_id<T>()) {
        func(static_cast<T *>(thing), std::forward<Args>(args)...);
      }
    }
  }

  template<typename T, typename Fnc, typename...Args>
  void WithAllThingsOfType(Fnc func, Args&& ... args) {
    for (auto& thing : _things) {
      if (thing->type() == type_id<T>()) {
        func(static_cast<T *>(thing), std::forward<Args>(args)...);
      }
    }
  }

  template<typename T, typename Fnc, typename...Args>
  void WithAllComponents(Fnc func, Args&& ...args) {
    for (auto& thing : _things) {
      if (auto cmp = thing->get_component<T>()) {
        func(thing, cmp, std::forward<Args>(args)...);
      }
    }
  }

  template<typename T, typename Fnc, typename...Args>
  void WithAllComponents(Fnc func, Args&& ...args) const {
    for (const auto& thing : _things) {
      if (const auto cmp = thing->get_component<T>()) {
        func(thing, cmp, std::forward<Args>(args)...);
      }
    }
  }

};
}
