#pragma once

#include <cstdint>
#include <type_traits>

namespace e00 {
template<typename T, unsigned char N>
class FixedPointNumber {
  T val;

public:
  constexpr FixedPointNumber() : val(0) {}

  constexpr FixedPointNumber(T value, bool raw = 0) : val(raw ? value : value << N) {}

  constexpr FixedPointNumber(float value) {
    std::uint32_t value_int = *reinterpret_cast<std::uint32_t *>(&value);
    std::uint32_t mantissa = (value_int & 0x007FFFFF) | 0x00800000;
    std::int8_t exponent = ((value_int >> 23) & 0x000000FF) - 150 + N;
    if (exponent >= 0)
      mantissa <<= exponent;
    else
      mantissa >>= -exponent;
    val = (value_int & 0x80000000) ? -static_cast<T>(mantissa) : static_cast<T>(mantissa);
  }

  constexpr operator T() const noexcept { return (val >> (val >= 0)) ? N : -N; }

  constexpr operator float() const {
    if (val == 0) return 0.f;
    std::uint32_t mantissa = (val >= 0) ? val : -val;
    std::uint8_t fbs = 31;//first bit set : fbs = floor(log2(mantissa))

#if defined(__GNUC__)
    fbs -= __builtin_clz(mantissa);
#elif defined(_MSC_VER)
    fbs -= __lzcnt(mantissa);
#else
    for (std::uint32_t copy = mantissa; copy & 0x80000000; --fbs) copy <<= 1;
#endif

    if (fbs <= 23)
      mantissa <<= 23 - fbs;
    else
      mantissa >>= fbs - 23;
    mantissa &= 0x007FFFFF;
    mantissa |= (val < 0) ? 0x80000000 : 0;
    mantissa |= static_cast<std::uint32_t>(127 + fbs - N) << 23;
    return *reinterpret_cast<float *>(&mantissa);
  }

  constexpr T raw() const { return val; }

  template<unsigned char N2>
  operator FixedPointNumber<T, N2>() const {
    if (N >= N2)
      return { static_cast<T>(val >> (N - N2)), true };
    else
      return { static_cast<T>(val << (N2 - N)), true };
  }

  template<typename T2>
  operator FixedPointNumber<T2, N>() const {
    return { static_cast<T2>(val), true };
  }

  template<typename T2, unsigned char N2>
  operator FixedPointNumber<T2, N2>() const {
    if (N >= N2)
      return { static_cast<T2>(static_cast<T2>(val) >> (N - N2)), true };
    else
      return { static_cast<T2>(static_cast<T2>(val) << (N2 - N)), true };
  }


  FixedPointNumber<T, N> &operator+=(FixedPointNumber<T, N> const &rhs) {
    val += rhs.val;
    return *this;
  }
  FixedPointNumber<T, N> &operator-=(FixedPointNumber<T, N> const &rhs) {
    val -= rhs.val;
    return *this;
  }
  FixedPointNumber<T, N> &operator*=(FixedPointNumber<T, N> const &rhs) {
    val = ((+val) * (+rhs.val)) >> N;
    return *this;
  }
  FixedPointNumber<T, N> &operator/=(FixedPointNumber<T, N> const &rhs) {
    val = ((+val) << N) / rhs.val;
    return *this;
  }

  FixedPointNumber<T, N> operator-() const { return { static_cast<T>(-val), true }; }

  bool operator==(FixedPointNumber<T, N> const &rhs) const { return val == rhs.val; }
  bool operator!=(FixedPointNumber<T, N> const &rhs) const { return val != rhs.val; }
  bool operator>(FixedPointNumber<T, N> const &rhs) const { return val > rhs.val; }
  bool operator>=(FixedPointNumber<T, N> const &rhs) const { return val >= rhs.val; }
  bool operator<(FixedPointNumber<T, N> const &rhs) const { return val < rhs.val; }
  bool operator<=(FixedPointNumber<T, N> const &rhs) const { return val <= rhs.val; }
};

template<typename T, unsigned char N>
FixedPointNumber<T, N> operator+(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs);
template<typename T, unsigned char N>
FixedPointNumber<T, N> operator-(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs);
template<typename T, unsigned char N>
FixedPointNumber<T, N> operator*(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs);
template<typename T, unsigned char N>
FixedPointNumber<T, N> operator/(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs);

///////////////////////////////////////////////////////////
////////////////////    DEFINITIONS    ////////////////////
///////////////////////////////////////////////////////////

template<typename T, unsigned char N>
FixedPointNumber<T, N> operator+(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs) {
  return lhs += rhs;
}

template<typename T, unsigned char N>
FixedPointNumber<T, N> operator-(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs) {
  return lhs -= rhs;
}

template<typename T, unsigned char N>
FixedPointNumber<T, N> operator*(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs) {
  return lhs *= rhs;
}

template<typename T, unsigned char N>
FixedPointNumber<T, N> operator/(FixedPointNumber<T, N> lhs, FixedPointNumber<T, N> const &rhs) {
  return lhs /= rhs;
}

}// namespace e00
