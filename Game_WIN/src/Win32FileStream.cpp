#include "Win32FileStream.hpp"

Win32FileStream::Win32FileStream(HANDLE fileHandle)
  : Stream(GetFileSize(fileHandle, nullptr)),
    _file(fileHandle) {
}

Win32FileStream::~Win32FileStream() {
  CloseHandle(_file);
}

std::error_code Win32FileStream::real_read(size_t size, void *data) {
  DWORD actual = 0;
  if (ReadFile(_file, data, size, &actual, nullptr)) {
    return {};
  }
  return std::make_error_code(std::errc::io_error);
}

std::error_code Win32FileStream::real_seek(size_t size) {
  if (SetFilePointer(
      _file,
      size,
      nullptr,
      FILE_BEGIN)
      == INVALID_SET_FILE_POINTER) {
    return std::make_error_code(std::errc::io_error);
  }
  return {};
}
