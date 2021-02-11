#pragma once

#include <Engine/Math/Vec2.hpp>

namespace e00 {
template<typename T>
struct Rect final {
  typedef T type;

  T x, y, width, height;

  Rect() : x(0), y(0), width(0), height(0) {}

  Rect(T x, T y, T w, T h) : x(x), y(y), width(w), height(h) {}

  Rect(const Vec2<T> &from, const Vec2<T> &size) : x(from.x()), y(from.y()), width(size.x()), height(size.y()) {}

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

  Rect operator+(const Vec2<T> &rhs) const {
    return Rect<T>(x + rhs.x(), y + rhs.y(), width, height);
  }

  Vec2<T> size() const { return { width, height }; }

  [[nodiscard]] Vec2<T> from() const noexcept { return { x, y }; }

  [[nodiscard]] Vec2<T> to() const noexcept { return Vec2<T>(x + width, y + height); }
};

}// namespace e00
