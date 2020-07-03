#pragma once

#include <windows.h>
#include <Engine/ResourceStream.hpp>

class Win32FileStream : public ResourceStream {
  HANDLE _file;

public:
  explicit Win32FileStream(HANDLE fileHandle);

  ~Win32FileStream() override;

  bool seek(size_t size) override;

  size_t current_position() override;

  size_t read(size_t max_size, void *data) override;
};


