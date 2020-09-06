#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <memory>

#include <Logger/LoggerSink.hpp>
#include <Logger/SourceLocation.hpp>
#include <Logger/Format.hpp>

namespace e00::impl {
class Logger final {
  std::string _name;
  std::vector<sys::LoggerSink *> _sinks;

public:
  enum class lvl {
    info,
    debug,
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
  typename std::enable_if<sizeof...(Args) != 0, void>::type log(source_location srcloc, lvl level, std::string_view fmt, Args &&... args) const {
    // Do we actually need to log ?
    if (!should_log(level)) {
      return;
    }

    const auto result = fmt_lite::format(fmt, std::forward<Args>(args)...);
    _real_log(srcloc, level, result);
  }

  template<typename... Args>
  typename std::enable_if<sizeof...(Args) == 0, void>::type log(source_location srcloc, lvl level, std::string_view fmt) const {
    // Do we actually need to log ?
    if (!should_log(level)) {
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

  template<typename... Args>
  void debug(source_location srcloc, std::string_view fmt, Args &&... args) const {
    log(srcloc, lvl::debug, fmt, std::forward<Args>(args)...);
  }

private:
  void _real_log(const source_location &srcloc, lvl level, std::string_view fmt) const;

  void _sink(lvl level, std::string_view data) const {
    for (const auto &s : _sinks) {
      switch (level) {
        case lvl::info:
          s->info(data);
          break;
        case lvl::debug: break;
        case lvl::error:
          s->error(data);
          break;
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
 * @param logger the logger to initialize
 */
void logger_initialize(Logger *logger);

/**
 * Adds a global sink
 *
 * @param sink the sink to add
 */
void logger_add_root_sink(sys::LoggerSink *sink);
}// namespace e00::impl
