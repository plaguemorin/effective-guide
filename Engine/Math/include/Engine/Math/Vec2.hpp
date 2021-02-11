#pragma once

#include "Vec.hpp"

namespace e00 {

template<typename T>
struct Vec2 : Vec<T, 2> {
  Vec2() : Vec<T, 2>() {}

  template<typename Q>
  Vec2(const Q &x, const Q &y) : Vec<T, 2>{ static_cast<T>(x), static_cast<T>(y) } {}

  Vec2(const Vec2<T> &rhs) : Vec<T, 2>(std::forward<decltype(rhs)>(rhs)) {}

  Vec2(Vec2<T> &&other) noexcept : Vec<T, 2>(std::forward<decltype(other)>(other)) {}

  explicit Vec2(const Vec<T, 2>& s) noexcept : Vec<T, 2>{ static_cast<T>(s.at(0)), static_cast<T>(s.at(1)) } {}

  Vec2(const std::initializer_list<T> &lst) : Vec<T, 2>{ lst } {}

  // Assignment
  Vec2<T> &operator=(const Vec2<T> &rhs) {
    copy(rhs);
    return *this;
  }

  Vec2<T> &operator=(Vec2<T> &&rhs) noexcept {
    swap(rhs);
    return *this;
  }

  auto total() const noexcept { return at(0) * at(1); }

  const auto &x() const { return at(0); }
  auto &x() { return at(0); }

  const auto &y() const { return at(1); }
  auto &y() { return at(1); }

  Vec2<T> flip() const noexcept { return Vec2<T>{ y(), x() }; }
};

template<typename T>
Vec2<T> operator/(const Vec2<T> &a, const Vec2<T> &b) {
  return Vec2<T>(a.x() / b.x(), a.y() / b.y());
}

template<typename T>
Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b) {
  return Vec2<T>(a.x() + b.x(), a.y() + b.y());
}

template<typename T>
Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b) {
  return Vec2<T>(a.x() - b.x(), a.y() - b.y());
}

}// namespace e00
