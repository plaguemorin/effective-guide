#pragma once

#include <array>

#include <Engine/TypedObject.hpp>

namespace thing {
class Component : public TypedObject {
protected:
  explicit Component(type_t type_id) : TypedObject(type_id) {}

public:
  // Leave virtual destructor: ComponentContainer deletes using the base class
  virtual ~Component() = default;
};

template<typename T>
class ComponentT : public Component {
public:
  ComponentT() : Component(type_id<T>()) {}

  // Leave virtual destructor: ComponentContainer deletes using the base class
  ~ComponentT() override = default;
};

class ComponentContainer {
  std::array<Component *, 25> _components;

  template<typename ComponentType>
  constexpr void validate() const {
    static_assert(std::is_convertible<ComponentType *, Component *>::value, "Class must be subclass of Component");
  }

  [[nodiscard]] Component *get_component(const type_t& id) const;

  bool add_component(Component *cmp);

protected:
  ComponentContainer();

  ~ComponentContainer();

public:
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
        delete static_cast<T *>(i);
        i = nullptr;
      }
    }
  }

};
}
