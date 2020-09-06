#pragma once

#include <string>
#include <Engine/System/RootStreamFactory.hpp>

class Win32StreamFactory : public e00::sys::RootStreamFactory {
  std::string _base_path;
public:

  explicit Win32StreamFactory(std::string base_path) : _base_path(std::move(base_path)) {
    // Make sure _base_path ends with a '\'
    if (!_base_path.empty()) {
      auto last_char = _base_path.at(_base_path.size() - 1);
      if (!(last_char == '\\' || last_char == '/')) {
        _base_path.append("\\");
      }
    }
  }

  ~Win32StreamFactory() override = default;

  std::unique_ptr<e00::Stream> open_stream(const std::string &fileName) override;

  std::unique_ptr<e00::sys::StreamFactory> load_pack(const std::string &pack_name) override;
};
