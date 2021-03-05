#pragma once

#include <cstdint>
#include <vector>
#include <system_error>

namespace e00 {
/*constexpr resource_file_type_t invalid_type = 0;

constexpr resource_file_type_t to_type(uint8_t b, uint8_t c, uint8_t d) {
  return (
    (0x2Eu << 24u)
    + (static_cast<resource_file_type_t>(b << 16u))
    + (static_cast<resource_file_type_t>(c << 8u))
    + d);
}

constexpr resource_file_type_t to_type(const uint8_t id[3]) {
  return to_type(id[0], id[1], id[2]);
}*/

class Stream {
  const size_t _stream_size;
  size_t _current_position;

protected:
  explicit Stream(size_t stream_size) : _stream_size(stream_size), _current_position(0) {}

  virtual std::error_code real_read(size_t size, void *data) = 0;

  virtual std::error_code real_seek(size_t /*unused*/) { return {}; }

public:
  Stream(Stream &&other) noexcept = delete;

  Stream(const Stream &other) = delete;

  Stream &operator=(Stream &&) = delete;

  Stream &operator=(const Stream &) = delete;

  virtual ~Stream() = default;

  [[nodiscard]] size_t stream_size() const { return _stream_size; }

  [[nodiscard]] bool end_of_stream() const { return current_position() >= stream_size(); }

  [[nodiscard]] size_t current_position() const { return _current_position; }

  [[nodiscard]] size_t max_read() const { return _stream_size - _current_position; }

  /**
   * Seeks to an absolute position
   *
   * @param new_position the absolute position to seek to
   * @return error code of any error that occurred, if any
   */
  std::error_code seek(size_t new_position) {
    if (new_position <= _stream_size) {
      if (const auto ec = real_seek(new_position)) {
        return ec;
      }
      _current_position = new_position;
      return {};
    }

    return std::make_error_code(std::errc::value_too_large);
  }

  std::error_code read(size_t max_size, void *data) {
    if (max_size > max_read()) {
      return std::make_error_code(std::errc::value_too_large);
    }

    if (const auto ec = real_read(max_size, data)) {
      //reset position
      real_seek(_current_position);
      return ec;
    }

    _current_position += max_size;
    return {};
  }

  template<typename T>
  std::error_code read(std::vector<T> &data) { return read(data.size() * sizeof(T), data.data()); }

  /**
   * Reads until \n
   *
   * @param max_length the max length of the string
   * @return the line
   */
  std::string read_line(size_t max_length = 0xFFFF) {
    if (end_of_stream())
      return {};

    std::string result;

    while (result.length() < max_length) {
      // Exit this loop if we're at EOF
      if (end_of_stream())
        break;

      char c;
      // If we can't read a char, exit
      if (!read(c))
        break;

      // skip returns
      if (c == '\r')
        continue;

      // if we have a new line, quit this loop
      if (c == '\n')
        break;

      result.push_back(c);
    }

    return result;
  }

  /**
   * Read a line into a char buffer
   *
   * @param str the buffer to use
   * @param max_length max length of the buffer
   * @return the buffer, or nullptr if an error occured
   */
  char *read_line_into(char *str, int max_length) {
    if (max_length <= 0)
      return nullptr;

    if (end_of_stream())
      return nullptr;

    char *p = str;
    char *end = str + max_length;

    // Read until we hit \n or until there's no buffer space left
    while (p != end) {
      // Exit this loop if we're at EOF
      if (end_of_stream())
        break;

      char c;
      // If we can't read a char, exit
      if (!read(c))
        break;

      // skip returns
      if (c == '\r')
        continue;

      // if we have a new line, quit this loop
      if (c == '\n')
        break;

      *p++ = c;
    }

    // set NULL terminator and exit
    *p = 0;
    return str;
  }

  template<typename T, unsigned N>
  bool read(T (&t)[N]) {
    return !read(sizeof(T) * N, t);
  }

  template<typename T>
  bool read(T &out) { return !read(sizeof(T), &out); }

  template<typename T>
  T read() {
    T read_value;
    if (read(read_value)) {
      return read_value;
    }
    return {};
  }
};
}// namespace e00
