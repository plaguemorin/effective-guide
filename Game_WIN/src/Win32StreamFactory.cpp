#include <windows.h>

#include "Win32StreamFactory.hpp"
#include "Win32FileStream.hpp"

#include "string_2_wstring.hpp"

std::unique_ptr<e00::Stream> Win32StreamFactory::open_stream(const std::string &fileName) {
    // Load up file from the resource folder
    auto wstr = s2ws(_base_path + fileName);
    auto *f = CreateFile(
      wstr.c_str(),
      GENERIC_READ,
      0,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr
    );

    if (f == INVALID_HANDLE_VALUE) {
      // Well, screw this!
      return nullptr;
    }

    return std::make_unique<Win32FileStream>(f);
}

std::unique_ptr<e00::sys::StreamFactory> Win32StreamFactory::load_pack(const std::string &pack_name) {
  // We need to find pack_name + ".pak" as directory
  return std::unique_ptr<e00::sys::StreamFactory>(new Win32StreamFactory(_base_path + pack_name + ".pak\\"));
}
