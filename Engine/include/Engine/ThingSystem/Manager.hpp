#pragma once

#include <Engine/ThingSystem/Thing.hpp>
#include <Engine/TypedObjectContainer.hpp>

namespace e00::thing {
class Manager
  : public impl::TypedObjectContainer<Thing> {
public:
  Manager() = default;

  ~Manager() = default;

  [[nodiscard]] Thing* at(uint16_t tile_x, uint16_t tile_y) const;

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
  void all_components(Fnc func, Args&& ...args) const {
    for (const auto& thing : container()) {
      if (!thing) continue;
      if (const auto cmp = thing->get_component<T>()) {
        func(thing, cmp, std::forward<Args>(args)...);
      }
    }
  }

};
}
