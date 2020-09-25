#pragma once

#include <type_traits>

#include <Engine/ResourceSystem/Resource.hpp>

namespace e00::resource {
class Manager;

class ResourcePtrData {
  friend class Manager;

  mutable Manager *_manager;
  id_t _id;
  Resource *_resource;

  void ref() const;
  void unref() const;

protected:
  ResourcePtrData(Manager *manager_, Resource *resource_)
    : _manager(manager_),
      _id(resource_->id()),
      _resource(resource_) {
    ref();
  }

  ResourcePtrData(Manager *manager_, id_t id_)
    : _manager(manager_),
      _id(id_),
      _resource(nullptr) {
    ref();
  }

  ResourcePtrData(const ResourcePtrData &other)
    : _manager(other._manager),
      _id(other._id),
      _resource(other._resource) {
    ref();
  }

  ResourcePtrData(ResourcePtrData &&other) noexcept
    : _manager(other._manager),
      _id(other._id),
      _resource(other._resource) {
    ref();
  }

  virtual ~ResourcePtrData() { unref(); }

  void copy(const ResourcePtrData &rhs) {
    unref();
    _manager = rhs._manager;
    _id = rhs._id;
    _resource = rhs._resource;
    ref();
  }

  void move(ResourcePtrData &&other) noexcept {
    unref();
    _manager = other._manager;
    _id = other._id;
    _resource = other._resource;
    ref();

    other.unref();
    other._id = 0;
    other._manager = nullptr;
    other._resource = nullptr;
  }

  template<typename T>
  const T *Get() const {
    static_assert(std::is_convertible<T, Resource>::value, "Class must be subclass of Resource");
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
      "T must be a non-const, non-volatile value_type");

    return static_cast<T *>(_resource);
  }

  template<typename T>
  T *Get() {
    static_assert(std::is_convertible<T, Resource>::value, "Class must be subclass of Resource");
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
      "T must be a non-const, non-volatile value_type");

    return static_cast<T *>(_resource);
  }

public:
  ResourcePtrData() : _manager(nullptr), _id(0), _resource(nullptr) {}

  [[nodiscard]] bool valid() const { return _id != 0 && _manager != nullptr; }

  [[nodiscard]] const Manager *manager() const { return _manager; }
  Manager *manager() { return _manager; }

  explicit operator bool() const { return valid() && _resource; }
};

template<typename ResourceType>
class ResourcePtr : public ResourcePtrData {
  friend class Manager;

  static_assert(std::is_convertible<ResourceType, Resource>::value, "Class must be subclass of Resource");
  static_assert(std::is_same<typename std::remove_cv<ResourceType>::type, ResourceType>::value,
    "T must be a non-const, non-volatile value_type");

protected:
  ResourcePtr(Manager *manager_, id_t id_)
    : ResourcePtrData(manager_, id_) {}

  ResourcePtr(Manager *manager_, ResourceType *resource)
    : ResourcePtrData(manager_, resource) {}

public:
  ResourcePtr(std::nullptr_t) : ResourcePtrData() {}

  ResourcePtr() : ResourcePtr(nullptr) {}

  ResourcePtr(const ResourcePtr<ResourceType> &other) : ResourcePtrData(other) {}

  ~ResourcePtr() override = default;


  ResourcePtr<ResourceType> &operator=(const ResourcePtr<ResourceType> &rhs) {
    if (&rhs != this) {
      copy(rhs);
    }

    return *this;
  }

  const ResourceType *operator->() const { return Get<ResourceType>(); }
  ResourceType *operator->() { return Get<ResourceType>(); }

  const ResourceType *operator*() const { return Get<ResourceType>(); }
  ResourceType *operator*() { return Get<ResourceType>(); }
};
}// namespace e00::resource
