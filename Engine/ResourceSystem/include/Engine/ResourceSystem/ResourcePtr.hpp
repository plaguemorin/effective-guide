#pragma once

#include <type_traits>

#include <Engine/ResourceSystem/Resource.hpp>

namespace e00::resource {
class Manager;

class ResourcePtrData {
  friend class Manager;

  mutable Manager *_manager;
  id_t _id;

  void ref() const;
  void unref() const;

protected:
  ResourcePtrData() : _manager(nullptr), _id(0) {}

  ResourcePtrData(Manager *manager_, id_t id_) : _manager(manager_), _id(id_) {
    ref();
  }

  ResourcePtrData(const ResourcePtrData &other) : _manager(other._manager), _id(other._id) {
    ref();
  }

  ResourcePtrData(ResourcePtrData &&other) noexcept : _manager(other._manager), _id(other._id) {
    ref();

    other.unref();
    other._manager = nullptr;
    other._id = 0;
  }

  virtual ~ResourcePtrData() { unref(); }

  void copy(const ResourcePtrData &rhs) {
    unref();
    _manager = rhs._manager;
    _id = rhs._id;
    ref();
  }

  void move(ResourcePtrData &&other) {
    unref();
    _manager = other._manager;
    _id = other._id;
    ref();

    other.unref();
    other._id = 0;
    other._manager = nullptr;
  }

public:
  [[nodiscard]] bool valid() const { return _id != 0 && _manager != nullptr; }

  [[nodiscard]] const Manager *manager() const { return _manager; }
  Manager *manager() { return _manager; }
};

template<typename ResourceType>
class ResourcePtr : public ResourcePtrData {
  friend class Manager;

  static_assert(std::is_convertible<ResourceType, Resource>::value, "Class must be subclass of Resource");
  static_assert(std::is_same<typename std::remove_cv<ResourceType>::type, ResourceType>::value,
    "T must be a non-const, non-volatile value_type");

  ResourceType *_resource;

protected:
  ResourcePtr(Manager *manager_, id_t id_)
    : ResourcePtrData(manager_, id_) {}

  ResourcePtr(Manager *manager_, ResourceType *resource)
    : ResourcePtrData(manager_, resource->id()),
      _resource(resource) {}

public:
  ResourcePtr(std::nullptr_t) : ResourcePtrData(), _resource(nullptr) {}

  ResourcePtr() : ResourcePtr(nullptr) {}

  ResourcePtr(const ResourcePtr<ResourceType> &other) : ResourcePtrData(other), _resource(other._resource) {}

  ~ResourcePtr() override = default;

  explicit operator bool() const { return valid() && _resource; }

  ResourcePtr<ResourceType> &operator=(const ResourcePtr<ResourceType> &rhs) {
    if (&rhs != this) {
      copy(rhs);
      _resource = rhs._resource;
    }

    return *this;
  }

  const ResourceType *operator->() const { return _resource; }
  ResourceType *operator->() { return _resource; }

  const ResourceType *operator*() const { return _resource; }
  ResourceType *operator*() { return _resource; }

  template<typename T>
  ResourcePtr<T> As() const {
    static_assert(std::is_convertible<T, ResourceType>::value, "Class must be subclass of Resource");
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
      "T must be a non-const, non-volatile value_type");

    // Runtime checks ?
    return ResourcePtr(manager(), static_cast<T *>(_resource));
  }
};
}// namespace e00::resource
