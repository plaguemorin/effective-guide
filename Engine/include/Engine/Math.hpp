#pragma once

#include <array>
#include <algorithm>
#include <type_traits>
#include <cstdint>

namespace e00 {
template<typename T>
struct Vec2 {
  typedef T type;

  type x, y;

  Vec2(T _x, T _y) noexcept : x(_x), y(_y) {}

  Vec2() noexcept : x(0), y(0) {}

  Vec2(const Vec2<T> &other) = default;

  Vec2<T> flip() const { return { y, x }; }

  bool operator>(const Vec2<T> &rhs) const { return x > rhs.x && y > rhs.y; }

  bool operator>=(const Vec2<T> &rhs) const { return x >= rhs.x && y >= rhs.y; }

  bool operator<(const Vec2<T> &rhs) const { return x < rhs.x && y < rhs.y; }

  bool operator<=(const Vec2<T> &rhs) const { return x <= rhs.x && y <= rhs.y; }

  Vec2<T> operator/(const Vec2<T> &rhs) const { return { x / rhs.x, y / rhs.y }; }

  Vec2<T> operator+(const Vec2<T> &rhs) const { return { x + rhs.x, y + rhs.y }; }

  template<typename Number>
  typename std::enable_if<std::is_arithmetic<Number>::value, Vec2<Number>>::type operator/(Number n) const { return { x / n, y / n }; }

  Vec2<T> operator%(const Vec2<T> &rhs) const { return { x % rhs.x, y % rhs.y }; }

  auto total() const { return x * y; }
};

template<typename T>
struct Vec3 {
  typedef T type;

  type x, y, z;

  Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

  Vec3(const Vec3<T> &other) : x(other.x), y(other.y), z(other.z) {}

  explicit Vec3(T n) : x(n), y(n), z(n) {}

  template<typename Z>
  typename std::enable_if<std::is_arithmetic<Z>::value, Vec3<Z>>::type operator/(const Z &rhs) const {
    return {
      static_cast<Z>(x) / rhs,
      static_cast<Z>(y) / rhs,
      static_cast<Z>(z) / rhs
    };
  }

  template<typename Z>
  typename std::enable_if<std::is_arithmetic<Z>::value, Vec3<Z>>::type operator*(const Z &rhs) const {
    return {
      static_cast<Z>(x) * rhs,
      static_cast<Z>(y) * rhs,
      static_cast<Z>(z) * rhs
    };
  }

  template<typename Z>
  Vec3<T> operator*(const Vec3<Z> &rhs) const {
    return { x * rhs.x, y * rhs.y, z * rhs.z };
  }

  template<typename Z>
  Vec3<T> operator-(const Vec3<Z> &rhs) const {
    return {
      x - rhs.x,
      y - rhs.y,
      z - rhs.z
    };
  }

  template<typename Z>
  Vec3<T> operator+(const Vec3<Z> &rhs) const {
    return {
      x + static_cast<T>(rhs.x),
      y + static_cast<T>(rhs.y),
      z + static_cast<T>(rhs.z)
    };
  }

  template<typename Z>
  Vec3<T> &operator+=(const Vec3<Z> &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  [[nodiscard]] Vec3<T> clamp(T min, T max) const {
    T nx = x, ny = y, nz = z;

    if (nx < min)
      nx = min;
    else if (nx > max)
      nx = max;
    if (ny < min)
      ny = min;
    else if (ny > max)
      ny = max;
    if (nz < min)
      nz = min;
    else if (nz > max)
      nz = max;

    return { nx, ny, nz };
  }

  template<typename Z, Z min = std::numeric_limits<Z>::min(), Z max = std::numeric_limits<Z>::max()>
  [[nodiscard]] Vec3<Z> clamp() const {
    T nx = x, ny = y, nz = z;

    if (nx < min)
      nx = min;
    else if (nx > max)
      nx = max;
    if (ny < min)
      ny = min;
    else if (ny > max)
      ny = max;
    if (nz < min)
      nz = min;
    else if (nz > max)
      nz = max;

    return Vec3<Z>(
      static_cast<Z>(nx),
      static_cast<Z>(ny),
      static_cast<Z>(nz));
  }

  [[nodiscard]] T sum() const { return x + y + z; }
};

template<typename T>
struct Rect {
  typedef T type;

  T x, y, width, height;

  static Rect rect_from_points(const Vec2<T> &from, const Vec2<T> &to) {
    Rect<T> r;
    if (to.x > from.x) {
      r.x = from.x;
      r.width = to.x - from.x;
    } else {
      r.x = to.x;
      r.width = from.x - to.x;
    }

    if (to.y > from.y) {
      r.y = from.y;
      r.height = to.y - from.y;
    } else {
      r.y = to.y;
      r.height = from.y - to.y;
    }

    return r;
  }

  [[nodiscard]] bool within(const Vec2<T> &point) const {
    return point > from() && point < to();
  }

  Vec2<T> size() const { return { width, height }; }

  [[nodiscard]] Vec2<T> from() const noexcept { return { x, y }; }

  [[nodiscard]] Vec2<T> to() const noexcept { return { x + width, y + height }; }
};

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  Color() : red(0), green(0), blue(0) {}

  Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

  explicit Color(const Vec3<uint8_t> &src) : red(src.x), green(src.y), blue(src.z) {}

  explicit Color(uint32_t rgb)
    : red((rgb >> 16u) & 0xFFu),
      green((rgb >> 8u) & 0xFFu),
      blue(rgb & 0xFFu) {}

  explicit operator uint32_t() const {
    return static_cast<uint32_t>((red << 16u) + (green << 8u) + blue);
  }

  bool operator==(const Color &rhs) const {
    return (red == rhs.red)
           && (green == rhs.green)
           && (blue == rhs.blue);
  }

  template<typename T>
  [[nodiscard]] typename std::enable_if<std::is_arithmetic<T>::value, Vec3<T>>::type vec3() const {
    return {
      static_cast<T>(red),
      static_cast<T>(green),
      static_cast<T>(blue)
    };
  }

  [[nodiscard]] Vec3<int> diff(const Color &rhs) const {
    return {
      red - rhs.red,
      green - rhs.green,
      blue - rhs.blue
    };
  }

  [[nodiscard]] Vec3<double> diff_normalized(const Color &rhs) const {
    return {
      (red - rhs.red) / 255.,
      (green - rhs.green) / 255.,
      (blue - rhs.blue) / 255.
    };
  }

  /**
   * Compare the difference of two RGB values, weigh by CCIR 601 luminosity:
   *
   * @param c2 color to compare to
   * @return 0 to 1.0 on how different the color is
   */
  [[nodiscard]] double compare(const Color &c2) const {
    const auto lumadiff = luma_normalized() - c2.luma_normalized();
    const auto normalized = diff_normalized(c2);

    return (normalized * normalized * Vec3<double>(0.299, 0.587, 0.114)).sum() * 0.75
           + lumadiff * lumadiff;
  }

  /**
   * Luma value, CCIR 601 weighted
   *
   * @return 0 to 255000
   */
  [[nodiscard]] uint32_t luma() const {
    return red * 299u + green * 587u + blue * 114u;
  }

  /**
   * Normalized luma value
   * See #luma()
   *
   * @return 0.0 to 1.0
   */
  [[nodiscard]] double luma_normalized() const {
    return luma() / (255.0 * 1000);
  }

  Color &operator+=(const Color &rhs) {
    red = static_cast<uint8_t>(red + rhs.red);
    green = static_cast<uint8_t>(green + rhs.green);
    blue = static_cast<uint8_t>(blue + rhs.blue);

    return *this;
  }

  Color operator-(const Color &rhs) const {
    return Color(
      static_cast<uint8_t>(red - rhs.red),
      static_cast<uint8_t>(green - rhs.green),
      static_cast<uint8_t>(blue - rhs.blue));
  }
};

template<std::size_t MatrixSize>
struct PaletteMixingPlan {
  std::array<uint8_t, MatrixSize> color_index;

  [[nodiscard]] std::size_t matrix_size() const { return MatrixSize; }
};

template<std::size_t ColorCount>
struct Palette {
  std::array<Color, ColorCount> colors;

  [[nodiscard]] uint8_t color_count() const { return ColorCount; }

  std::array<uint32_t, ColorCount> compute_palette_luma() const {
    std::array<uint32_t, ColorCount> result;
    for (std::size_t i = 0; i < ColorCount; i++)
      result[i] = colors[i].luma();

    return result;
  }

  template<std::size_t MatrixSize>
  PaletteMixingPlan<MatrixSize> devise_best_mixing_plan(const Color &src, const double errorMul = 0.09) const {
    PaletteMixingPlan<MatrixSize> result = {};

    Vec3<int> error_accumulator(0);

    for (std::size_t c = 0; c < MatrixSize; ++c) {
      const auto current_temporary_value = Color((src.vec3<int>() + error_accumulator * errorMul).clamp<uint8_t>());

      // Find the closest color from the palette
      const auto chosen = find_closest_color(current_temporary_value);
      result.color_index[c] = chosen;
      error_accumulator += (src - colors[chosen]).template vec3<int>();
    }

    // Sort the colors according to luminance
    const auto luma = compute_palette_luma();

    std::sort(
      result.color_index.begin(),
      result.color_index.end(),
      [&](const uint8_t &a, const uint8_t &b) -> bool {
        return luma[a] < luma[b];
      });

    return result;
  }

  [[nodiscard]] uint8_t find_closest_color(const Color &t) const {
    // Find the closest color from the palette
    double least_penalty = 1e99;
    std::size_t chosen = 0;// c % ColorCount;

    for (std::size_t index = 0; index < ColorCount; ++index) {
      const Color &pc = colors[index];

      const auto penalty = pc.compare(t);
      if (penalty < least_penalty) {
        least_penalty = penalty;
        chosen = index;
      }
    }

    return static_cast<uint8_t>(chosen);
  }
};

using RectI = Rect<int>;
using Vec3I = Vec3<int>;
using Vec2I = Vec2<int>;
using Vec2UI = Vec2<uint8_t>;

}// namespace e00impl
