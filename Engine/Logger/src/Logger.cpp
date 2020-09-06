#include <Logger/Logger.hpp>

namespace {
std::vector<e00::sys::LoggerSink *> default_sinks;
std::unique_ptr<e00::impl::Logger> root_logger;
}// namespace

namespace e00::impl {
void Logger::_real_log(const source_location &srcloc, Logger::lvl level, std::string_view fmt) const {
  (void)srcloc;
  _sink(level, fmt);

  for (auto *sink : default_sinks) {
    if (level == lvl::info) {
      sink->info(fmt);
    } else {
      sink->error(fmt);
    }
  }
}

bool Logger::should_log(Logger::lvl level) const {
  (void)level;
  return true;
}

Logger *logger() {
  if (root_logger == nullptr) {
    root_logger.reset(new Logger("ROOT"));
  }

  return root_logger.get();
}

void logger_add_root_sink(sys::LoggerSink *sink) {
  default_sinks.push_back(sink);
}

void logger_initialize(Logger *logger) {
  (void)logger;
}
}// namespace e00impl
