#pragma once

#include <cstdint>

namespace e00 {
struct ColorHSV final {
  uint8_t hue; // this uses 0-255 and not 0-360 as commonly found for hue
  uint8_t saturation;
  uint8_t value;

  constexpr ColorHSV() : hue(0), saturation(0), value(0) {}

  ColorHSV(uint8_t h, uint8_t s, uint8_t v) : hue(h), saturation(s), value(v) {}

  ColorHSV(const ColorHSV &other) = default;
};

struct Color final {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  constexpr Color() : red(0), green(0), blue(0) {}

  Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

  explicit Color(const ColorHSV &hsv) {
    if (hsv.saturation == 0) {
      red = hsv.value;
      green = hsv.value;
      blue = hsv.value;
    } else {
      // Each "region" is about 60deg (or 43 if you base it in 255)
      uint8_t region = hsv.hue / 43;
      uint8_t remainder = (hsv.hue - (region * 43)) * 6;
      uint8_t p = (hsv.value * (255 - hsv.saturation)) >> 8;
      uint8_t q = (hsv.value * (255 - ((hsv.saturation * remainder) >> 8))) >> 8;
      uint8_t t = (hsv.value * (255 - ((hsv.saturation * (255 - remainder)) >> 8))) >> 8;

      switch (region) {
        case 0:
          red = hsv.value;
          green = t;
          blue = p;
          break;
        case 1:
          red = q;
          green = hsv.value;
          blue = p;
          break;
        case 2:
          red = p;
          green = hsv.value;
          blue = t;
          break;
        case 3:
          red = p;
          green = q;
          blue = hsv.value;
          break;
        case 4:
          red = t;
          green = p;
          blue = hsv.value;
          break;
        default:
          red = hsv.value;
          green = p;
          blue = q;
          break;
      }
    }
  }

  explicit Color(uint32_t rgb)
    : Color(
      ((rgb & 0x00FF0000u) >> 16u) & 0xFF,
      ((rgb & 0x0000FF00u) >> 8u) & 0xFF,
      (rgb & 0x000000FFu)) {}

  Color &operator=(const Color &rhs) {
    if (&rhs != this) {
      red = rhs.red;
      green = rhs.green;
      blue = rhs.blue;
    }
    return *this;
  }

  void set(uint8_t r, uint8_t g, uint8_t b) noexcept {
    this->red = r;
    this->green = g;
    this->blue = b;
  }

  [[nodiscard]] uint8_t get_red() const noexcept { return red; }
  [[nodiscard]] uint8_t get_green() const noexcept { return green; }
  [[nodiscard]] uint8_t get_blue() const noexcept { return blue; }

  [[nodiscard]] uint8_t get_rgb_min() const noexcept {
    return red < green ? (red < blue ? red : blue) : (green < blue ? green : blue);
  }

  [[nodiscard]] uint8_t get_rgb_max() const noexcept {
    return red > green ? (red > blue ? red : blue) : (green > blue ? green : blue);
  }

  [[nodiscard]] ColorHSV to_hsv() const noexcept {
    const auto max = get_rgb_max();
    if (max == 0) {
      return ColorHSV();
    }

    const auto min = get_rgb_min();
    ColorHSV ret(0, 255 * long(max - min) / max, max);

    if (ret.saturation != 0) {
      // Each "region" is about 60deg (or 43 if you base it in 255)
      if (max == red) {
        ret.hue = 0 + 43 * (green - blue) / (max - min);
      } else if (max == green) {
        ret.hue = 85 + 43 * (blue - red) / (max - min);
      } else {
        ret.hue = 171 + 43 * (red - green) / (max - min);
      }
    }

    return ret;
  }

  /**
   * Luma value, CCIR 601 weighted
   *
   * @return 0 to 255000
   */
  [[nodiscard]] uint32_t luma() const noexcept {
    return red * 299u + green * 587u + blue * 114u;
  }

  explicit operator uint32_t() const noexcept {
    return static_cast<uint32_t>((red << 16u) + (green << 8u) + blue);
  }

  bool operator==(const Color &rhs) const noexcept {
    return (red == rhs.red)
           && (green == rhs.green)
           && (blue == rhs.blue);
  }
};
}// namespace e00
