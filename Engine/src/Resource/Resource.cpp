#include "Engine/Resource/Resource.hpp"

namespace e00::resource {
Resource::Resource(type_t type)
  : TypedObject(type),
    _id{} {
}

Resource::Resource(type_t type, std::string_view name) : NamedObject(name), TypedObject(type), _id{} {
}

Resource::Resource(resource_id_t resource_id, type_t type) : TypedObject(type), _id(resource_id) {
}
Resource::Resource(resource_id_t resource_id, type_t type, std::string_view name) : NamedObject(name), TypedObject(type), _id(resource_id) {
}

}// namespace e00::resource
