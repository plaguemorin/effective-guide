#include <Engine/Resource.hpp>

namespace e00::resource {
Resource::Resource(type_t type)
  : TypedObject(type),
    _id{} {
}

Resource::Resource(type_t type, std::string name)
  : NamedObject(std::move(name)), TypedObject(type), _id{} {
}

Resource::Resource(id_t resource_id, type_t type)
  : TypedObject(type), _id(resource_id) {
}

Resource::Resource(id_t resource_id, type_t type, std::string name)
  : NamedObject(std::move(name)), TypedObject(type), _id(resource_id) {
}

}// namespace e00::resource
