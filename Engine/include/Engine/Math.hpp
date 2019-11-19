#pragma once

template<typename T>
struct Point {
  typedef T type;

  T x;
  T y;

  Point(T x, T y) noexcept : x(x), y(y) {}

  Point() noexcept : x(0), y(0) {}

  Point(const Point<T>& other) = default;
};

template<typename T>
struct Size {
  typedef T type;

  T x;
  T y;

  Size(T x, T y) noexcept : x(x), y(y) {}

  Size() noexcept : x(0), y(0) {}

  Size(const Size& other) = default;

  bool operator==(const Size<T>& rhs) const {
    return x == rhs.x && y == rhs.y;
  }
};

template<typename T>
struct Rect {
  typedef T type;

  T x, y, width, height;

  [[nodiscard]] Point<T> From() const noexcept { return {x, y}; }

  [[nodiscard]] Point<T> To() const noexcept { return {x + width, y + height}; }
};

using PointI = Point<int>;
using SizeI = Size<int>;
using RectI = Rect<int>;
