#pragma once

#include <Engine/Math/Vec2.hpp>

namespace e00 {
template<typename T>
struct Rect final {
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

}
