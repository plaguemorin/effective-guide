#pragma once

#include <type_traits>

namespace e00 {
template<typename T>
struct Vec2 final {
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
}
