#pragma once

#include <cstdint>

namespace e00 {
struct Color final {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

  Color() : Color(0, 0, 0) {}

  explicit Color(uint32_t rgb)
    : Color(
    ((rgb & 0x00FF0000u) >> 16u) & 0xFF,
    ((rgb & 0x0000FF00u) >> 8u) & 0xFF,
    (rgb & 0x000000FFu)
  ) {}

  Color& operator=(const Color& rhs) {
    if (&rhs != this) {
      red = rhs.red;
      green = rhs.green;
      blue = rhs.blue;
    }
    return *this;
  }

  /**
   * Luma value, CCIR 601 weighted
   *
   * @return 0 to 255000
   */
  [[nodiscard]] uint32_t luma() const {
    return red * 299u + green * 587u + blue * 114u;
  }

  explicit operator uint32_t() const {
    return static_cast<uint32_t>((red << 16u) + (green << 8u) + blue);
  }

  bool operator==(const Color &rhs) const {
    return (red == rhs.red)
           && (green == rhs.green)
           && (blue == rhs.blue);
  }
};
}// namespace e00
