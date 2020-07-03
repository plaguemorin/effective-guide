#pragma once

#include <array>
#include <memory>
#include <type_traits>

#include <Engine/TypedObject.hpp>

namespace e00::thing {
class Component : public TypedObject {
protected:
  explicit Component(type_t type_id) : TypedObject(type_id) {}

public:
  // Leave the virtual destructor: ComponentContainer deletes using the base class
  virtual ~Component() = default;
};

template<typename T>
class ComponentT : public Component {
public:
  ComponentT() : Component(type_id<T>()) {}

  // Leave the virtual destructor: ComponentContainer deletes using the base class
  ~ComponentT() override = default;
};

class ComponentContainer {
  std::array<Component *, 25> _components;

  template<typename ComponentType>
  constexpr void validate() const {
    static_assert(std::is_convertible<ComponentType *, Component *>::value, "Class must be subclass of Component");
  }

  [[nodiscard]] Component *get_component(const type_t& id) const;

protected:
  ComponentContainer();

  ~ComponentContainer();

public:
  template<typename T, typename FuncType, typename RetType = std::invoke_result_t<FuncType, T&>>
  RetType with_component(FuncType&& func, const RetType& default_value = {}) const {
    validate<T>();

    if (auto t = get_component<T>())
      return func(*t);

    return default_value;
  }

  bool add_component(std::unique_ptr<Component>&& component) {
    const auto type_id = component->type();
    auto free_spot = _components.size();

    // Make sure it's unique and find first free spot
    for (auto i = _components.size() - 1; i != 0; i--) {
      if (!_components[i])
        free_spot = i;
      else if (_components[i]->type() == type_id)
        return false;
    }

    if (free_spot < _components.size()) {
      _components[free_spot] = component.release();
      return true;
    }

    return false;
  }

  template<typename T, typename ...Args>
  T *create_component(Args&& ...args) {
    validate<T>();

    auto free_spot = _components.size();

    // Make sure it's unique and find first free spot
    for (auto i = _components.size(); i > 0; i--) {
      if (!_components[i - 1])
        free_spot = i - 1;
      else if (_components[i - 1]->type() == type_id<T>())
        return nullptr;
    }

    if (free_spot < _components.size()) {
      _components[free_spot] = new T(std::forward<Args>(args)...);
      return static_cast<T *>(_components[free_spot]);
    }

    return nullptr;
  }

  template<typename T>
  T *get_component() const {
    validate<T>();

    for (auto& i : _components) {
      if (i && i->type() == type_id<T>())
        return static_cast<T *>(i);
    }

    return nullptr;
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
