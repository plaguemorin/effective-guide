#pragma once

#include <array>

namespace e00 {
template<std::size_t MatrixSize>
struct PaletteMixingPlan {
  std::array<uint8_t, MatrixSize> color_index;

  [[nodiscard]] std::size_t matrix_size() const { return MatrixSize; }
};
}
