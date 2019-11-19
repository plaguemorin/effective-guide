#include "Engine/Resource/Resource.hpp"

namespace resource {

Resource::Resource(type_t type)
        : TypedObject(type),
          _id{} {


}

Resource::Resource() : Resource(type_id<Resource>()) {}

}
