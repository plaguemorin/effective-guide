#pragma once

#include <Engine/ThingSystem/Thing.hpp>
#include <Engine/TypedObjectContainer.hpp>

namespace thing {
class Manager : public TypedObjectContainer<Thing> {
public:
  Manager() = default;

  ~Manager() override = default;

  template<typename T, typename Fnc, typename...Args>
  void all_components(Fnc func, Args&& ...args) {
    for (auto& thing : container()) {
      if (!thing) continue;
      if (auto cmp = thing->get_component<T>()) {
        func(thing, cmp, std::forward<Args>(args)...);
      }
    }
  }

  template<typename T, typename Fnc, typename...Args>
  void WithAllComponents(Fnc func, Args&& ...args) const {
    for (const auto& thing : container()) {
      if (!thing) continue;
      if (const auto cmp = thing->get_component<T>()) {
        func(thing, cmp, std::forward<Args>(args)...);
      }
    }
  }

};
}
