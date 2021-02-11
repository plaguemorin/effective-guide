#pragma once

#include <array>
#include <algorithm>

namespace e00 {
namespace detail {
  template<typename Tp, std::size_t Nm>
  struct vector_traits {
    typedef Tp Type[Nm];
    static constexpr Tp &ref(const Type &t, std::size_t n) noexcept { return const_cast<Tp &>(t[n]); }
    static constexpr Tp *ptr(const Type &t) noexcept { return const_cast<Tp *>(t); }
  };
}// namespace detail

template<typename T, std::size_t N>
struct Vec {
  typedef T value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  typedef detail::vector_traits<T, N> Vector_Traits;
  typename Vector_Traits::Type _vector_elements;

  Vec() = default;

  explicit Vec(const value_type &u) { fill(std::forward<value_type>(u)); }

  Vec(const Vec<T, N>& rhs) { copy(rhs); }

  Vec(const std::initializer_list<T> &lst) : _vector_elements() {
    std::copy_n(lst.begin(), std::min(N, lst.size()), begin());
  }

  Vec(Vec<T, N> &&other) noexcept { copy(other); }

  void copy(const Vec<T, N> &rhs) { std::copy_n(rhs.begin(), N, begin()); }

  void fill(const value_type &u) { std::fill_n(begin(), N, u); }

  void swap(Vec<T, N> &other) {
    for (size_type i = 0; i < N; i++) std::swap(_vector_elements[i], other._vector_elements[i]);
  }

  auto magnitude_squared() const noexcept {
    T t = T();
    for (size_type i = 0; i < N; i++) t += at(i) * at(i);
    return t;
  }

  auto magnitude() const noexcept {
    return sqrtf(magnitude_squared());
  }

  constexpr auto sum() const noexcept {
    T t = T();
    for (size_type i = 0; i < N; i++) t += at(i);
    return t;
  }

  constexpr auto dot(const Vec<T, N> &rhs) const noexcept {
    T t = T();
    for (size_type i = 0; i < N; i++) t += at(i) * rhs.at(i);
    return t;
  }


  // Operators
  Vec<T, N> operator+(const Vec<T, N> &rhs) const {
    Vec<T, N> ret;
    for (size_type i = 0; i < N; i++) ret[i] = at(i) + rhs.at(i);
    return ret;
  }
  Vec<T, N> operator-(const Vec<T, N> &rhs) const {
    Vec<T, N> ret;
    for (size_type i = 0; i < N; i++) ret[i] = at(i) - rhs.at(i);
    return ret;
  }
  Vec<T, N> operator*(const Vec<T, N> &rhs) const {
    Vec<T, N> ret;
    for (size_type i = 0; i < N; i++) ret[i] = at(i) * rhs.at(i);
    return ret;
  }
  Vec<T, N> operator/(const Vec<T, N> &rhs) const {
    Vec<T, N> ret;
    for (size_type i = 0; i < N; i++) ret[i] = at(i) / rhs.at(i);
    return ret;
  }
  Vec<T, N> operator%(const Vec<T, N> &rhs) const {
    Vec<T, N> ret;
    for (size_type i = 0; i < N; i++) ret[i] = at(i) % rhs.at(i);
    return ret;
  }

  Vec<T, N> &operator+=(const Vec<T, N> &rhs) {
    for (size_type i = 0; i < N; i++) _vector_elements[i] += rhs.at(i);
    return *this;
  }
  Vec<T, N> &operator-=(const Vec<T, N> &rhs) {
    for (size_type i = 0; i < N; i++) _vector_elements[i] -= rhs.at(i);
    return *this;
  }
  Vec<T, N> &operator*=(const Vec<T, N> &rhs) {
    for (size_type i = 0; i < N; i++) _vector_elements[i] *= rhs.at(i);
    return *this;
  }
  Vec<T, N> &operator/=(const Vec<T, N> &rhs) {
    for (size_type i = 0; i < N; i++) _vector_elements[i] /= rhs.at(i);
    return *this;
  }
  Vec<T, N> &operator%=(const Vec<T, N> &rhs) {
    for (size_type i = 0; i < N; i++) _vector_elements[i] %= rhs.at(i);
    return *this;
  }

  // Casting.
  template<typename T2>
  constexpr explicit operator Vec<T2, N>() const noexcept {
    Vec<T2, N> r;
    for (size_type i = 0; i < N; i++) r[i] = static_cast<T2>(at(i));
    return r;
  }

  // Assignment
  Vec<T, N> &operator=(const Vec<T, N> &rhs) {
    copy(rhs);
    return *this;
  }

  Vec<T, N> &operator=(Vec<T, N> &&rhs) noexcept {
    swap(rhs);
    return *this;
  }

  // Capacity.
  [[nodiscard]] constexpr size_type size() const noexcept { return N; }
  [[nodiscard]] constexpr size_type max_size() const noexcept { return N; }
  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  // Iterators
  constexpr iterator begin() noexcept { return iterator(data()); }
  constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
  constexpr iterator end() noexcept { return iterator(data() + N); }
  constexpr const_iterator end() const noexcept { return const_iterator(data() + N); }
  constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
  constexpr const_iterator cend() const noexcept { return const_iterator(data() + N); }

  // Element access
  constexpr reference operator[](size_type n) noexcept { return Vector_Traits::ref(_vector_elements, n); }
  constexpr const_reference operator[](size_type n) const noexcept { return Vector_Traits::ref(_vector_elements, n); }
  constexpr reference at(size_type n) { return Vector_Traits::ref(_vector_elements, n); }
  constexpr const_reference at(size_type n) const { return Vector_Traits::ref(_vector_elements, n); }
  constexpr reference front() noexcept { return *begin(); }
  constexpr const_reference front() const noexcept { return Vector_Traits::ref(_vector_elements, 0); }
  constexpr reference back() noexcept { return N ? *(end() - 1) : *end(); }
  constexpr const_reference back() const noexcept { return Vector_Traits::ref(_vector_elements, N - 1); }
  constexpr pointer data() noexcept { return Vector_Traits::ptr(_vector_elements); }
  constexpr const_pointer data() const noexcept { return Vector_Traits::ptr(_vector_elements); }

  // Comparators
  bool operator>(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) <= rhs.at(i)) return false;
    return true;
  }

  bool operator>=(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) < rhs.at(i)) return false;
    return true;
  }

  bool operator<(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) >= rhs.at(i)) return false;
    return true;
  }

  bool operator<=(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) > rhs.at(i)) return false;
    return true;
  }

  bool operator==(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) != rhs.at(i)) return false;
    return true;
  }

  bool operator!=(const Vec<T, N> &rhs) const noexcept {
    for (size_type i = 0; i < N; i++)
      if (at(i) == rhs.at(i)) return false;
    return true;
  }
};


}// namespace e00
