#pragma once

#include <array>
#include <Engine/ThingSystem/TypedObject.hpp>

namespace thing {
class Component : public TypedObject {
protected:
  explicit Component(thing_type_t type_id) : TypedObject(type_id) {}

public:
  ~Component() override = default;
};

template<typename T>
class ComponentT : public Component {
public:
  ComponentT() : Component(type_id<T>()) {}

  ~ComponentT() override = default;
};

class ComponentContainer {
  std::array<Component *, 25> _components;

  template<typename ComponentType>
  constexpr void validate() const {
    static_assert(std::is_convertible<ComponentType *, Component *>::value, "Class must be subclass of Component");
  }

  [[nodiscard]] Component *get_component(const thing_type_t& id) const;

  bool add_component(Component *cmp);

public:
  ComponentContainer();

  virtual ~ComponentContainer();

  template<typename T, typename ...Args>
  T *add_component(Args&& ...args) {
    validate<T>();

    // Make sure it's unique
    if (get_component(type_id<T>())) {
      // If we found a component with the same type id;
      // then it's not unique, abort!
      return nullptr;
    }

    // Try to add it to our list
    auto new_component = new T(std::forward<Args>(args)...);
    if (!add_component(new_component)) {
      // Something went wrong (maybe we ran out of space?)
      delete new_component;
      return nullptr;
    }

    return new_component;
  }

  template<typename T>
  T *get_component() const {
    validate<T>();
    return static_cast<T *>(get_component(type_id<T>()));
  }

  template<typename T>
  void remove_component() {
    validate<T>();
    for (auto& i :_components) {
      if (i && i->type() == type_id<T>()) {
        delete i;
        i = nullptr;
      }
    }
  }

};
}
