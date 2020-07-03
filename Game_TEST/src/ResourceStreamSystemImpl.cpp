#include "ResourceStreamSystemImpl.hpp"

ResourceStreamSystemImpl::ResourceStreamSystemImpl(const std::string& filename) {
  _file = fopen(filename.c_str(), "rb");
}

ResourceStreamSystemImpl::~ResourceStreamSystemImpl() {
  fclose(_file);
}

size_t ResourceStreamSystemImpl::stream_size() {
  auto current = ftell(_file);
  fseek(_file, 0, SEEK_END);
  auto file_size = ftell(_file);
  fseek(_file, current, SEEK_SET);

  if (file_size > 0) {
    return static_cast<size_t>(file_size);
  }

  return 0;
}

bool ResourceStreamSystemImpl::seek(size_t size) {
  return fseek(_file, static_cast<long>(size), SEEK_SET) == 0;
}

size_t ResourceStreamSystemImpl::read(size_t max_size, void *data) {
  auto read = fread(data, max_size, 1, _file);
  if (read > 0) {
    return read;
  }

  return 0;
}
