#pragma once

#include <array>
#include <cstdint>

#include <Engine/CommonTypes.hpp>
#include <Engine/NamedObject.hpp>
#include <Engine/TypedObject.hpp>

namespace e00::resource {
constexpr resource_id_t to_id(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  return static_cast<resource_id_t>((a << 24U) + (b << 16U) + (c << 8U) + d);
}

constexpr resource_id_t to_id(const std::array<uint8_t, 4> &id) {
  return to_id(id[0], id[1], id[2], id[3]);
}

class Resource
  : public NamedObject
  , public TypedObject {
  resource_id_t _id;

protected:
  // Use type_id<T>() to generate
  explicit Resource(type_t type);

  Resource(type_t type, std::string_view name);

  Resource(resource_id_t resource_id, type_t type);

  Resource(resource_id_t resource_id, type_t type, std::string_view name);

public:
  Resource() = delete;

  Resource(Resource &&) noexcept = default;

  Resource(const Resource &) = delete;

  virtual ~Resource() = default;

  Resource& operator=(const Resource& rhs) = delete;

  Resource& operator=(Resource&& other) noexcept = delete;

  bool operator==(const Resource &rhs) const { return _id == rhs._id; }
};
}// namespace e00::resource
