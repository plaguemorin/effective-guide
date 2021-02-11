#pragma once

#include <cstdint>

namespace e00 {
template<typename T, std::size_t N, std::size_t M>
struct Matrix final {
  typedef T type;

  type m[N][M];
};
}// namespace e00
