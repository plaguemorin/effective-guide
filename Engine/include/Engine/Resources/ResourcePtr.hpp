#pragma once

#include <type_traits>
#include <utility>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include <Engine/Resource.hpp>

namespace e00 {
class Engine;
}

namespace e00::resource {
namespace detail {
  class ControlBlock {
    uint16_t _count;
    void *_ptr;
    std::string _name;
    Engine *_loader;

  public:
    ControlBlock(void *ptr, std::string name, Engine *eng) noexcept
      : _count(0),
        _ptr(ptr),
        _name(std::move(name)),
        _loader(eng) {}

    void increment() noexcept { _count++; }
    void decrement() noexcept { _count--; }

    void replacePtr(void *ptr) { _ptr = ptr; }

    [[nodiscard]] decltype(_count) count() const noexcept { return _count; }
    [[nodiscard]] decltype(_loader) loader() const noexcept { return _loader; }
    [[nodiscard]] const decltype(_name) &name() const noexcept { return _name; }
    [[nodiscard]] void *get() const noexcept { return _ptr; }
  };

  class ControlBlockObject {
    detail::ControlBlock *_control_block;
    type_t _type;

    void _perform_lazy_load();

  protected:
    constexpr ControlBlockObject(type_t type) noexcept : _control_block(nullptr), _type(type) {}

    explicit ControlBlockObject(void *p, std::string name, Engine *eng, type_t type) noexcept
      : _control_block(new detail::ControlBlock(p, std::move(name), eng)),
        _type(type) {
      increment();
    }

    ControlBlockObject(const ControlBlockObject &other) noexcept
      : _control_block(other._control_block), _type(other._type) {
      increment();
    }

    ControlBlockObject(ControlBlockObject &&other) noexcept : _control_block(nullptr), _type(other._type) {
      std::swap(_control_block, other._control_block);
    }

    virtual ~ControlBlockObject() {
      if (_control_block) {
        _control_block->decrement();
        if (_control_block->count() == 0) {
          delete _control_block;
        }
      }

      _control_block = nullptr;
    }

    void setPtr(void *ptr) {
      if (_control_block != nullptr && _control_block->get() == nullptr) {
        _control_block->replacePtr(ptr);
      }
    }

    template<typename T>
    // Return the stored pointer.
    T *raw() const noexcept { return static_cast<T *>(_control_block ? _control_block->get() : nullptr); }

    void swap(ControlBlockObject &right) {
      std::swap(_control_block, right._control_block);
    }

    void increment() {
      if (_control_block) _control_block->increment();
    }

    void decrement() {
      if (_control_block) _control_block->decrement();
    }

  public:
    // Usage counter
    [[nodiscard]] uint16_t use_count() const noexcept { return _control_block ? _control_block->count() : 0; }
    [[nodiscard]] const auto *loader() const { return _control_block ? _control_block->loader() : nullptr; }
    [[nodiscard]] std::string name() const { return _control_block ? _control_block->name() : "(No CB)"; }
    [[nodiscard]] type_t contained_type() const { return _type; }
    [[nodiscard]] bool is_loaded() const { return _control_block != nullptr && _control_block->get() != nullptr; }

    // Return true if this is a valid reference
    explicit operator bool() const noexcept { return _control_block != nullptr; }

    //
    void ensure_loaded() const {
      if (_control_block != nullptr && _control_block->get() == nullptr) {
        // OMG THIS IS UGLY
        (const_cast<ControlBlockObject *>(this))->_perform_lazy_load();
      }
    }
  };
}// namespace detail

/**
 * @brief A "smart" pointer for resources with reference-counted copy semantics.
 *
 * @tparam ResourceT
 */
template<typename ResourceT>
class ResourcePtr : public detail::ControlBlockObject {
  friend class e00::Engine;
  static_assert(std::is_convertible<ResourceT *, Resource *>::value, "Derived must inherit Resource as public");

public:
  using element_type = typename std::remove_extent<ResourceT>::type;

  /**
   *  @brief  Construct an empty %ResourcePtr.
   *  @post   use_count()==0 && get()==0
   */
  constexpr ResourcePtr() noexcept : detail::ControlBlockObject(type_id<ResourceT>()) {}

  constexpr ResourcePtr(nullptr_t) noexcept : detail::ControlBlockObject(type_id<ResourceT>()) {}

  ResourcePtr(const ResourcePtr &copy) noexcept : detail::ControlBlockObject(copy) {}

  ResourcePtr(ResourcePtr &&right) noexcept : detail::ControlBlockObject(std::move(right)) {}

  /**
   * For lazy loading a resource... just populate the contained_type and the name
   *
   * @param name name of future resource
   * @param type contained_type of future resource
   * @param eng loader engine
   */
  ResourcePtr(std::string name, type_t type, Engine *eng) noexcept
    : detail::ControlBlockObject(nullptr, std::move(name), eng, type) {
  }

  /**
   * Make a resource pointer to an already loaded pointer
   *
   * @tparam Yp resource contained_type
   * @param p resource pointer
   * @param eng loader engine
   */
  template<typename Yp>
  ResourcePtr(Yp *p, Engine *eng)
    : detail::ControlBlockObject(p, p ? p->name() : "", eng, p ? p->type() : type_id<Yp>()) {
  }

  /**
   * Alias `right` to be contained_type `ResourceT`
   *
   * @tparam Yp the original contained_type
   * @param right the original resource
   * @param ptr the casted pointer
   */
  template<typename Yp>
  ResourcePtr(const ResourcePtr<Yp> &right, element_type *ptr) noexcept
    : detail::ControlBlockObject(right) {
  }

  /**
   * Take over a resource loaded into a unique_ptr,
   *
   * @tparam Yp the resource contained_type
   * @param other the resource pointer to take over
   * @param eng the loader engine
   */
  template<typename Yp>
  ResourcePtr(std::unique_ptr<Yp> &&other, Engine *eng)
    : detail::ControlBlockObject(nullptr, other->name(), eng, other->type()) {
    setPtr(other.release());
  }

  ~ResourcePtr() override {
    if (use_count() == 1) {
      delete raw<ResourceT>();
    }
  }

  void swap(ResourcePtr &right) noexcept {
    detail::ControlBlockObject::swap(right);
  }

  void reset() noexcept {
    ResourcePtr().swap(*this);
  }

  template<class OtherResourceT>
  void reset(OtherResourceT *otherPointerT) {// release, take ownership of otherPointerT
    ResourcePtr(otherPointerT).swap(*this);
  }

  //
  ResourcePtr &operator=(const ResourcePtr &right) noexcept {
    ResourcePtr(right).swap(*this);
    return *this;
  }

  template<class OtherResourceT>
  ResourcePtr &operator=(const ResourcePtr<OtherResourceT> &right) noexcept {
    ResourcePtr(right).swap(*this);
    return *this;
  }

  ResourcePtr &operator=(ResourcePtr &&right) noexcept {// take resource from right
    ResourcePtr(std::move(right)).swap(*this);
    return *this;
  }

  template<class OtherResourceT>
  ResourcePtr &operator=(ResourcePtr<OtherResourceT> &&right) noexcept {// take resource from right
    ResourcePtr(std::move(right)).swap(*this);
    return *this;
  }

  element_type *get() const noexcept {
    ensure_loaded();
    return raw<ResourceT>();
  }

  template<typename OtherResourceT = ResourceT>
  OtherResourceT *operator->() const noexcept { return get(); }

  element_type *operator*() noexcept { return get(); }

  const element_type *operator*() const noexcept { return get(); }
};

///////////////////////////////////////////////////////
// Comparators
template<typename Tp, typename Up>
inline bool operator==(const ResourcePtr<Tp> &a, const ResourcePtr<Up> &b) noexcept { return a.get() == b.get(); }

template<typename Tp, typename Up>
inline bool operator!=(const ResourcePtr<Tp> &a, const ResourcePtr<Up> &b) noexcept { return a.get() != b.get(); }

// Compare with nullptr
template<typename Tp>
inline bool operator==(const ResourcePtr<Tp> &a, nullptr_t) noexcept { return !a; }

///////////////////////////////////////////////////////
// Creators
template<class ResourceT, class... Types>
ResourcePtr<ResourceT> make_resource_ptr(Types &&...Args) {
  return ResourcePtr<ResourceT>(new ResourceT(std::forward<Types>(Args)...));
}

template<class ResourceT, typename EngineT>
ResourcePtr<ResourceT> make_lazy_ptr(std::string name, type_t type, EngineT loader) {
  return ResourcePtr<ResourceT>(std::move(name), type, loader);
}

///////////////////////////////////////////////////////
// Casting
template<typename T, typename P>
ResourcePtr<T> resource_ptr_cast(const ResourcePtr<P> &source) {
#if 1
  if (!(source.contained_type() == type_id<T>())) {
    return nullptr;
  }
#endif

  // static_cast for ResourcePtr that properly respects the reference count control block
  const auto ptr = static_cast<typename ResourcePtr<T>::element_type *>(source.get());
  return ResourcePtr<T>(source, ptr);
}

}// namespace e00::resource
