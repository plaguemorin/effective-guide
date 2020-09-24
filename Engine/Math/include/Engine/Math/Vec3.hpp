#pragma once

#include <limits>
#include <type_traits>

namespace e00 {
template<typename T>
struct Vec3 final {
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
}
