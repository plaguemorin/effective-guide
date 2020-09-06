#pragma once

#include <array>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <string_view>
#include <string>
#include <utility>

#include <magic_enum.hpp>

namespace e00::fmt_lite {
const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};


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

template<>
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

template<>
constexpr unsigned int num_digits(char x) {
  if (x < 0)
    return num_digits(-x) + 1;
  if (x >= 100)
    return 3;
  if (x >= 10)
    return 2;
  return 1;
}

template<typename T>
struct formatter { formatter() = delete; };

template<typename T>
struct integer_formatter {
  template<typename Context>
  void parse(Context &ctx, T value) {
    char buffer[num_digits(std::numeric_limits<T>::max())];

    if (value < 0) {
      *ctx._out++ = '-';
      parse(ctx, static_cast<T>(-value));
      return;
    }

    const auto size = num_digits(value);
    char *c = buffer + size - 1;

    while (value >= 100) {
      auto pos = value % 100;
      value /= 100;
      *(short *)(c - 1) = *(short *)(digit_pairs + 2 * pos);
      c -= 2;
    }

    while (value > 0) {
      *c-- = static_cast<char>('0' + static_cast<char>(value % 10));
      value /= 10;
    }

    std::copy(buffer, buffer + size, ctx._out);
  }
};

template<>
struct formatter<int> : integer_formatter<int> { formatter() = default; };
template<>
struct formatter<char> : integer_formatter<char> { formatter() = default; };
template<>
struct formatter<unsigned int> : integer_formatter<unsigned int> { formatter() = default; };
template<>
struct formatter<unsigned long> : integer_formatter<unsigned long> { formatter() = default; };
template<>
struct formatter<long long> : integer_formatter<long long> { formatter() = default; };

template<>
struct formatter<bool> {
  formatter() = default;

  template<typename Context>
  void parse(Context &ctx, const bool &v) {
    std::string bool_value;

    if (v) {
      bool_value = "True";
    } else {
      bool_value = "False";
    }

    std::copy(bool_value.begin(), bool_value.end(), ctx._out);
  }
};

template<>
struct formatter<std::string_view> {
  formatter() = default;

  template<typename Context>
  void parse(Context &ctx, const std::string_view &v) {
    std::copy(v.begin(), v.end(), ctx._out);
  }
};

template<>
struct formatter<const void *> {
  formatter() = default;

  template<typename Context>
  void parse(Context &ctx, const void *value) {
    formatter<unsigned long>().parse(ctx, reinterpret_cast<unsigned long>(value));
  }
};

namespace internal {
  template<typename OutputIt>
  struct ArgFormatter {
    OutputIt _out;
    constexpr ArgFormatter(OutputIt it) : _out(it) {}

    template<typename T>
    constexpr typename std::enable_if<std::is_constructible<formatter<T>>::value, void>::type operator()(const T &i) {
      formatter<T>().parse(*this, i);
    }

    constexpr void operator()(long double) {}
  };

  enum class arg_type {
    none_type,
    int_type,
    uint_type,
    ulong_type,
    long_long_type,
    ulong_long_type,
    bool_type,
    char_type,
    float_type,
    double_type,
    long_double_type,
    cstring_type,
    string_type,
    pointer_type,
  };

  // Holds all types of possible arguments
  class format_arg {
    union {
      const int int_value;
      const unsigned uint_value;
      const long long long_long_value;
      const unsigned long ulong;
      const unsigned long long ulong_long_value;
      const bool bool_value;
      const char char_value;
      const float float_value;
      const double double_value;
      const long double long_double_value;
      const void *pointer;
      const std::string_view string;
    };

    const arg_type type;

  public:
    format_arg() : type(arg_type::none_type) {}
    explicit format_arg(std::string_view string_view) : string(string_view), type(arg_type::string_type) {}
    explicit format_arg(const char *string_value) : string(std::string_view(string_value)), type(arg_type::cstring_type) {}
    explicit format_arg(const void *ptr) : pointer(ptr), type(arg_type::pointer_type) {}
    explicit format_arg(long double ldv) : long_double_value(ldv), type(arg_type::long_double_type) {}
    explicit format_arg(double dv) : double_value(dv), type(arg_type::double_type) {}
    explicit format_arg(float fv) : float_value(fv), type(arg_type::float_type) {}
    explicit format_arg(char cv) : char_value(cv), type(arg_type::char_type) {}
    explicit format_arg(bool bv) : bool_value(bv), type(arg_type::bool_type) {}
    explicit format_arg(unsigned long long ullv) : ulong_long_value(ullv), type(arg_type::ulong_long_type) {}
    explicit format_arg(long long llv) : long_long_value(llv), type(arg_type::long_long_type) {}
    explicit format_arg(unsigned uiv) : uint_value(uiv), type(arg_type::uint_type) {}
    explicit format_arg(int iv) : int_value(iv), type(arg_type::int_type) {}
    explicit format_arg(unsigned long ul) : ulong(ul), type(arg_type::ulong_type) {}

    // Visits an arg by type
    template<typename Visitor>
    constexpr void visit_arg(Visitor &&vis) const {
      switch (type) {
        case arg_type::none_type: break;
        case arg_type::string_type: vis(string); break;
        case arg_type::ulong_type: vis(ulong); break;
        case arg_type::cstring_type: vis(string); break;
        case arg_type::pointer_type: vis(pointer); break;
        case arg_type::long_double_type: vis(long_double_value); break;
        case arg_type::double_type: vis(double_value); break;
        case arg_type::float_type: vis(float_value); break;
        case arg_type::char_type: vis(char_value); break;
        case arg_type::bool_type: vis(bool_value); break;
        case arg_type::ulong_long_type: vis(ulong_long_value); break;
        case arg_type::long_long_type: vis(long_long_value); break;
        case arg_type::uint_type: vis(uint_value); break;
        case arg_type::int_type: vis(int_value); break;
      }
    }
  };

  template<typename T>
  typename std::enable_if<!std::is_enum<T>::value, format_arg>::type make_arg(const T &arg) {
    return format_arg(arg);
  }

  template<typename T>
  typename std::enable_if<std::is_enum<T>::value, format_arg>::type make_arg(const T &arg) {
    return format_arg(magic_enum::enum_name<T>(arg));
  }

  // Holds a list of `format_arg`
  template<typename... Args>
  struct format_arg_store {
    static constexpr size_t num_args = sizeof...(Args);

    std::array<format_arg, num_args> data;

    format_arg_store(const Args &... args)
      : data{ make_arg(args)... } {}
  };

  // Used for passing format_arg_store around, without templates
  class basic_format_arg_store {
    int max_size;
    const format_arg *_args;

  public:
    template<typename... Args>
    explicit basic_format_arg_store(const format_arg_store<Args...> &store)
      : max_size(sizeof...(Args)) {
      _args = store.data.data();
    }

    [[nodiscard]] format_arg do_get(int index) const {
      if (index < max_size) {
        return _args[index];
      }
      return format_arg();
    }
  };

  // builds a `format_arg_store` that represents Args...
  template<typename... Args>
  inline format_arg_store<std::remove_reference_t<Args>...> make_arg_store(const std::string_view &format_str, const std::remove_reference_t<Args> &... args) {
    (void)format_str;
    return { args... };
  }

  // constexpr find
  template<typename T, typename Ptr = const T *>
  constexpr bool find(Ptr first, Ptr last, T value, Ptr &out) {
    for (out = first; out != last; ++out) {
      if (*out == value) {
        return true;
      }
    }
    return false;
  }

  template<typename OutputIt>
  inline void parse_format(OutputIt out, const std::string_view &fmt, basic_format_arg_store &&args) {
    int arg = 0;
    const char *begin = fmt.data();
    const char *end = begin + fmt.size();

    // Parse the format string
    while (begin != end) {
      const char *p = begin;

      // Check if we have a '{' in the rest of the string,
      if (*begin != '{' && !find(begin + 1, end, '{', p)) {
        // we don't so copy from begin to end to the output
        std::copy_n(begin, end - begin, out);
        return;
      }

      // We found '{', at p
      std::copy_n(begin, p - begin, out);
      ++p;

      // Make sure we don't end here
      if (p == end) {
        // ABORT !
        return;
      }

      auto argfmt = ArgFormatter(out);
      const auto argobj = args.do_get(arg);
      // Is this just a {} ?
      if (static_cast<char>(*p) == '}') {
        // Replace starting a p
        argobj.visit_arg(argfmt);
        arg++;
      } else {
        const char *end_bracket;
        // Arguments, but still replace at p
        if (!find(p, end, '}', end_bracket)) {
          return;
        }
        argobj.visit_arg(argfmt);
        p = end_bracket;

        arg++;
      }

      begin = p + 1;
    }
  }
}// namespace internal

template<typename... Args>
std::string format(const std::string_view &fmt, Args &&... args) {
  std::string output;

  // Output is at least as long as the input
  output.reserve(fmt.size());

  internal::parse_format(
    std::back_inserter(output),
    fmt,
    internal::basic_format_arg_store(internal::make_arg_store<Args...>(fmt, args...)));
  return output;
}
}// namespace e00::fmt_lite
