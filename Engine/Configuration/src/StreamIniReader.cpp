#include "INIReader.hpp"
#include <Engine/Configuration/Parser.hpp>

namespace {
class TrampolineReader : public e00::cfg::ParserListener {
  e00::cfg::ParserListener *_real_listener;
  std::error_code _last_ec;

public:
  TrampolineReader(ParserListener *real_listener) : _real_listener(real_listener), _last_ec() {}

  ~TrampolineReader() = default;

  std::error_code handle_entry(const e00::cfg::Entry &configuration_entry) override {
    return _last_ec = _real_listener->handle_entry(configuration_entry);
  }

  std::error_code get_last_error_code() const {
    return _last_ec;
  }
};

char *stream_ini_reader(char *str, int num, void *stream) {
  if (num <= 0)
    return nullptr;

  if (auto *s = static_cast<e00::Stream *>(stream)) {
    return s->read_line_into(str, num);
  }

  return nullptr;
}

int stream_ini_handler(void *user, const char *section, const char *name, const char *value) {
  if (auto *listener = static_cast<e00::cfg::ParserListener *>(user)) {
    e00::cfg::Entry ini_value;
    ini_value.section = std::string_view(section);
    ini_value.name = std::string_view(name);
    ini_value.value = std::string_view(value);

    auto ec = listener->handle_entry(ini_value);
    if (!ec) {
      return 1;
    }
  }

  return 0;
}
}// namespace

std::error_code e00::cfg::parse(const std::unique_ptr<Stream> &stream, cfg::ParserListener *listener) {
  TrampolineReader trampoline_reader(listener);

  const auto err = ini_parse_stream(
    &stream_ini_reader,
    stream.get(),
    &stream_ini_handler,
    &trampoline_reader);

  if (err != 0) {
    // TODO: Make real error code
    return std::make_error_code(std::errc::invalid_argument);
  }

  return trampoline_reader.get_last_error_code();
}
