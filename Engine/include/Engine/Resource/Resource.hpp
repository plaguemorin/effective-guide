#pragma once

#include <cstdint>

#include <Engine/NamedObject.hpp>
#include <Engine/TypedObject.hpp>

namespace resource {
using resource_id_t = uint32_t;

class Resource
        : public NamedObject,
          public TypedObject {
  resource_id_t _id;
protected:
  explicit Resource(type_t type);

public:
  Resource();

  virtual ~Resource() = default;
};
}
