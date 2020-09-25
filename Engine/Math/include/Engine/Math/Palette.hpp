#pragma once

#include <array>
#include <algorithm>
#include <type_traits>

#include <Engine/Math/Color.hpp>
#include <Engine/Math/PaletteMixingPlan.hpp>

namespace e00 {
template<std::size_t ColorCount>
struct Palette final {
  Color invalid;
  std::array<Color, ColorCount> colors;

  template<typename T>
  typename std::enable_if<std::is_integral_v<T>, Color &>::type operator[](T idx) {
    if (idx >= ColorCount) return invalid;

    if constexpr (std::is_signed<T>::value) {
      if (idx < 0) return invalid;
    }

    return colors.at(idx);
  }

  template<typename T>
  typename std::enable_if<std::is_integral_v<T>, const Color &>::type operator[](T idx) const {
    if (idx >= ColorCount) return invalid;

    if constexpr (std::is_signed<T>::value) {
      if (idx < 0) return invalid;
    }

    return colors.at(idx);
  }

  [[nodiscard]] uint8_t color_count() const { return ColorCount; }

  std::array<uint32_t, ColorCount> compute_palette_luma() const {
    std::array<uint32_t, ColorCount> result;
    for (std::size_t i = 0; i < ColorCount; i++) {
      result[i] = colors[i].luma();
    }

    return result;
  }

  /*  template<std::size_t MatrixSize>
  PaletteMixingPlan<MatrixSize> devise_best_mixing_plan(const Color &src, const double errorMul = 0.09) const {
    PaletteMixingPlan<MatrixSize> result = {};

    Vec3<int> error_accumulator(0);

    for (std::size_t c = 0; c < MatrixSize; ++c) {
      const auto current_temporary_value = Color((src.vec3<int>() + error_accumulator * errorMul).clamp<uint8_t>());

      // Find the closest color from the palette
      const auto chosen = find_closest_color(current_temporary_value);
      result.color_index[c] = chosen;
      error_accumulator += (src - colors[chosen]).template vec3<int>();
    }

    // Sort the colors according to luminance
    const auto luma = compute_palette_luma();

    std::sort(
      result.color_index.begin(),
      result.color_index.end(),
      [&](const uint8_t &a, const uint8_t &b) -> bool {
        return luma[a] < luma[b];
      });

    return result;
  }*/

  [[nodiscard]] uint8_t find_closest_color(const Color &t) const {
    // Find the closest color from the palette
    uint32_t least_penalty = std::numeric_limits<uint32_t>::max();
    std::size_t chosen = 0;// c % ColorCount;
    const auto t_luma = t.luma();

    for (std::size_t index = 0; index < ColorCount; ++index) {
      const Color &pc = colors[index];

      const auto luma = pc.luma();
      const auto penalty = (luma - t_luma) * (luma - t_luma);
      if (penalty < least_penalty) {
        least_penalty = penalty;
        chosen = index;
      }
    }

    return static_cast<uint8_t>(chosen);
  }
};
}// namespace e00
