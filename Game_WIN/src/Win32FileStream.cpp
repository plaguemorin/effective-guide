#include "Win32FileStream.hpp"

Win32FileStream::Win32FileStream(HANDLE fileHandle)
  : ResourceStream(GetFileSize(fileHandle, nullptr)),
    _file(fileHandle) {

}

Win32FileStream::~Win32FileStream() {
  CloseHandle(_file);
}

bool Win32FileStream::seek(size_t size) {
  return SetFilePointer(
          _file,
          size,
          nullptr,
          FILE_BEGIN) != INVALID_SET_FILE_POINTER;
}

size_t Win32FileStream::current_position() {
  return SetFilePointer(_file, 0, nullptr, FILE_CURRENT);
}

size_t Win32FileStream::read(size_t max_size, void *data) {
  DWORD actual;
  if (ReadFile(_file, data, max_size, &actual, nullptr)) {
    return actual;
  }
  return 0;
}
