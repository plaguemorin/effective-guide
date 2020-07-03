#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <memory>

#include <Engine/System/LoggerSink.hpp>
#include "StrFormat/Format.hpp"

namespace experimental {
struct source_location {
  static constexpr source_location current(const char *file, const char *func, uint_least32_t line, uint_least32_t col = 0) noexcept {
    return source_location(file, func, line, col);
  }

  constexpr source_location() noexcept : _file("unknown"), _func(_file), _line(0), _col(0) {}
  constexpr source_location(const char *file, const char *func, uint_least32_t line, uint_least32_t col = 0) noexcept : _file(file), _func(func), _line(line), _col(col) {}

  [[nodiscard]] constexpr uint_least32_t line() const noexcept { return _line; }
  [[nodiscard]] constexpr uint_least32_t column() const noexcept { return _col; }
  [[nodiscard]] constexpr const char *file_name() const noexcept { return _file; }
  [[nodiscard]] constexpr const char *function_name() const noexcept { return _func; }

private:
  const char *_file;
  const char *_func;
  const uint_least32_t _line;
  const uint_least32_t _col;
};
}// namespace experimental
using source_location = experimental::source_location;
#define SourceLocation() experimental::source_location::current(__FILE__, __FUNCTION__, __LINE__)

namespace e00::impl {
class Logger {
  std::string _name;
  std::vector<sys::LoggerSink *> _sinks;

public:
  enum class lvl {
    info,
    error
  };

  // Empty logger
  explicit Logger(std::string name) : _name(std::move(name)), _sinks() {}

  // Logger with range on sinks
  template<typename It>
  Logger(std::string name, It begin, It end) : _name(std::move(name)), _sinks(begin, end) {}

  Logger(std::string name, std::initializer_list<sys::LoggerSink *> sinks) : Logger(std::move(name), sinks.begin(), sinks.end()) {}

  Logger(std::string name, sys::LoggerSink *single_sink) : Logger(std::move(name), { single_sink }) {}

  ~Logger() = default;

  Logger(const Logger &other) : _name(other._name), _sinks(other._sinks) {}

  Logger(Logger &&other) noexcept : _name(std::move(other._name)), _sinks(std::move(other._sinks)) {}

  Logger &operator=(Logger other) noexcept {
    this->swap(other);
    return *this;
  }

  void swap(Logger &other) noexcept {
    _name.swap(other._name);
    _sinks.swap(other._sinks);
  }

  void add_sink(sys::LoggerSink *sink) {
    if (sink != nullptr) {
      _sinks.push_back(sink);
    }
  }

  void remove_sink(sys::LoggerSink *sink) {
    auto iterator = _sinks.begin();

    while (iterator != _sinks.end()) {
      if ((*iterator) == sink) {
        _sinks.erase(iterator);
        return;
      }
      iterator++;
    }
  }

  template<typename... Args>
  void log(source_location srcloc, lvl level, std::string_view fmt, Args &&... args) const {
    // Do we actually need to log ?
    if (!should_log(level)) {
      return;
    }

    if constexpr (sizeof...(Args) != 0) {
      const auto result = fmt_lite::format(fmt, std::forward<Args>(args)...);
      _real_log(srcloc, level, result);
      return;
    }

    _real_log(srcloc, level, fmt);
  }

  template<typename... Args>
  void info(source_location srcloc, std::string_view fmt, Args &&... args) const {
    log(srcloc, lvl::info, fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(source_location srcloc, std::string_view fmt, Args &&... args) const {
    log(srcloc, lvl::error, fmt, std::forward<Args>(args)...);
  }

private:
  void _real_log(const source_location &srcloc, lvl level, std::string_view fmt) const;

  void _sink(lvl level, std::string_view data) const {
    for (const auto &s : _sinks) {
      if (level == lvl::info) {
        s->info(data);
      } else {
        s->error(data);
      }
    }
  }

  [[nodiscard]] bool should_log(lvl level) const;
};

/**
 * Get the default logger
 *
 * @return
 */
Logger *logger();

/**
 * Initialize and register a logger
 * Useful for initializing created loggers with global settings.
 *
 * @param logger
 */
void logger_initialize(Logger *logger);

void logger_add_root_sink(sys::LoggerSink *sink);
}// namespace e00impl
