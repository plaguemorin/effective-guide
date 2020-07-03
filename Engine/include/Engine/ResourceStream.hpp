#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <system_error>

#include <Engine/CommonTypes.hpp>

namespace e00 {
constexpr resource_file_type_t invalid_type = 0;

constexpr resource_file_type_t to_type(uint8_t b, uint8_t c, uint8_t d) {
  return (
    (0x2Eu << 24u)
    + (static_cast<resource_file_type_t>(b << 16u))
    + (static_cast<resource_file_type_t>(c << 8u))
    + d);
}

constexpr resource_file_type_t to_type(const uint8_t id[3]) {
  return to_type(id[0], id[1], id[2]);
}

class ResourceStream {
  size_t _stream_size;

protected:
  explicit ResourceStream(size_t stream_size) : _stream_size(stream_size) {}

public:
  virtual ~ResourceStream() = default;

  size_t max_read() { return stream_size() - current_position(); }

  [[nodiscard]] size_t stream_size() const {
    return _stream_size;
  }

  virtual bool end_of_stream() { return current_position() >= stream_size(); }

  virtual bool seek(size_t) = 0;

  virtual size_t current_position() = 0;

  virtual size_t read(size_t max_size, void *data) = 0;

  virtual bool read(std::vector<uint8_t> &data) { return read(data.size(), data.data()) == data.size(); }

  template<typename T>
  bool read(T &out) { return read(sizeof(T), &out) == sizeof(T); }
};
}// namespace e00impl
