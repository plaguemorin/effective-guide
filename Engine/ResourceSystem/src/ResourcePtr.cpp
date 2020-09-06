#include <Engine/ResourceSystem/ResourcePtr.hpp>
#include <Engine/ResourceSystem/Manager.hpp>

namespace e00::resource {

void ResourcePtrData::ref() const {
  if (_manager) {
    _manager->add_reference(this);
  }
}

void ResourcePtrData::unref() const {
  if (_manager) {
    _manager->remove_reference(this);
  }
}

}// namespace e00::resource
