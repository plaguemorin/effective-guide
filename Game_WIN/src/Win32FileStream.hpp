#pragma once

#include <windows.h>
#include <Engine/Stream/Stream.hpp>

class Win32FileStream : public e00::Stream {
  HANDLE _file;

public:
  explicit Win32FileStream(HANDLE fileHandle);

  ~Win32FileStream() override;

protected:
  std::error_code real_read(size_t size, void *data) override;

  std::error_code real_seek(size_t size) override;
};
