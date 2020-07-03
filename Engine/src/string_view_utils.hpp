#pragma once

#include <string_view>
#include <system_error>

namespace e00 {
namespace detail {
  enum class ConversionErrc {
    // no 0
    NegativeNotAllowed = 1,
    BadCharacter,
    StringTooBig
  };
  std::error_code make_error_code(ConversionErrc);

  template<typename T>
  constexpr unsigned int num_digits(T number) {
    unsigned int digits = 0;

    if (number < 0)
      digits = 1;

    while (number) {
      number /= 10;
      digits++;
    }

    return digits;
  }

  constexpr unsigned int num_digits(int x) {
    if (x < 0)
      return num_digits(-x) + 1;

    if (x >= 10000) {
      if (x >= 10000000) {
        if (x >= 100000000) {
          if (x >= 1000000000)
            return 10;
          return 9;
        }

        return 8;
      }

      if (x >= 100000) {
        if (x >= 1000000)
          return 7;
        return 6;
      }

      return 5;
    }

    if (x >= 100) {
      if (x >= 1000)
        return 4;
      return 3;
    }

    if (x >= 10)
      return 2;
    return 1;
  }

  constexpr unsigned int num_digits(char x) {
    if (x < 0)
      return num_digits(-x) + 1;
    if (x >= 100)
      return 3;
    if (x >= 10)
      return 2;
    return 1;
  }

  template<typename T, std::size_t MaxDigits = num_digits(std::numeric_limits<T>::max())>
  bool can_fit(const std::string_view &sv) {
    return (sv.length() <= MaxDigits);
  }

  template<typename T>
  std::error_code stoi_fast_positive_only_1(const std::string_view &sv, T &out) {
    out = (*sv.begin()) & 0xF;
    return {};
  }

  template<typename T>
  std::error_code stoi_fast_positive_only_2(const std::string_view &sv, T &out) {
    const auto *n = reinterpret_cast<const uint16_t *>(sv.begin());

    // Digits are 8 bits wide
    // "25" is 13618 (0x3532)
    // String to in conversion is to AND with 0xF (last nibble as digits are 0x30 to 0x39)
    // 0x3532 & 0x0F0F = 0x0502
    // Flip and remove 0s
    // 0x0502 & 0xF00 >> 8 = 0x0005
    // 0x0502 & 0xF << 4 = 0x0020
    // 0x0005 + 0x0020 = 0x0025
    // This is now BDC (Binary coded decimal)

    const auto bcd = ((*n & 0xF00) >> 8) + ((*n & 0xF) << 4);
    out = static_cast<T>(bcd - 6 * (bcd >> 4));

    return {};
  }

  template<typename T>
  std::error_code stoi_fast_positive_only_3(const std::string_view &sv, T &out) {
    stoi_fast_positive_only_2(sv.substr(1), out);
    T tmp;
    stoi_fast_positive_only_1(sv, tmp);
    out = static_cast<T>(100 * tmp + out);
    return {};
  }

  template<typename T>
  std::error_code stoi_fast_positive_only_4(const std::string_view &sv, T &out) {
    const auto *n = reinterpret_cast<const uint32_t *>(sv.begin());

    const auto bcd =
      ((*n & 0xF000000) >> (24)) + ((*n & 0xF0000) >> (12)) + ((*n & 0xF00) >> (0)) + ((*n & 0xF) << (12));

    out = static_cast<T>((1000 * ((bcd & 0xF000) / (16 * 16 * 16))) + (100 * ((bcd & 0xF00) / (16 * 16))) + (10 * ((bcd & 0xF0) / 16)) + (1 * ((bcd & 0xF) / 1)));

    return {};
  }

  template<typename T>
  std::error_code stoi_fast_positive_only(const std::string_view &sv, T &out) {
    if (!can_fit<T>(sv)) {
      // OVERFLOW ?
      return make_error_code(ConversionErrc::StringTooBig);
    }

    switch (sv.length()) {
      case 0: out = 0; return {};
      case 1: return stoi_fast_positive_only_1(sv, out);
      case 2: return stoi_fast_positive_only_2(sv, out);
      case 3: return stoi_fast_positive_only_3(sv, out);
      case 4: return stoi_fast_positive_only_4(sv, out);
    }

    const char *end = sv.end();
    const char *p = sv.begin();

    out = 0;
    while (p != end) {
      const char i = *p++;
      if (i > '9' || i < '0') {
        return make_error_code(ConversionErrc::BadCharacter);
      }
      out = static_cast<T>((i & 0xF) + (10 * out));
    }

    return {};
  }
}// namespace detail

template<typename T>
typename std::enable_if<std::is_unsigned<T>::value, std::error_code>::type stoi_fast(const std::string_view &sv, T &out) {
  // This is unsigned conversion, so if it's negative exit outta here
  if (*sv.begin() == '-') {
    out = 0;
    return detail::make_error_code(detail::ConversionErrc::NegativeNotAllowed);
  }

  return detail::stoi_fast_positive_only(sv, out);
}

template<typename T>
typename std::enable_if<std::is_signed<T>::value, std::error_code>::type stoi_fast(const std::string_view &sv, T &out) {
  if (*sv.begin() == '-') {
    if (auto ec = detail::stoi_fast_positive_only(sv.substr(1), out)) {
      return ec;
    }
    out = -out;
    return {};
  }

  return detail::stoi_fast_positive_only(sv, out);
}

inline std::string_view trim(std::string_view s) {
  s.remove_prefix(std::min(s.find_first_not_of(" \t\r\v\n"), s.size()));
  s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t\r\v\n") - 1, s.size()));

  return s;
}

template<typename Func>
void execute_comma_separated(const std::string_view &input, Func &&func) {
  std::size_t from = 0;

  for (std::size_t i = 0; i < input.size(); ++i) {
    if (input[i] == ',') {
      func(trim(input.substr(from, i - from)));
      from = i + 1;
    }
  }

  if (from <= input.size()) {
    func(trim(input.substr(from)));
  }
}

template<typename OutputIt>
void split_comma_separated(const std::string_view &input, OutputIt out) {
  execute_comma_separated(input, [&out](const auto &sv) { *out++ = sv; });
}
}// namespace e00

namespace std {
template<>
struct is_error_code_enum<e00::detail::ConversionErrc> : true_type {};
}// namespace std
