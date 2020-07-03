#include "StdFile.hpp"

namespace {
size_t tell_size(FILE *fp) {
  if (std::fseek(fp, 0, SEEK_END) == 0) {
    const auto size = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    return static_cast<size_t>(size);
  }

  return 0;
}
}// namespace

std::unique_ptr<StdFile> StdFile::CreateFromFilename(const std::string &fileName) {
  if (FILE *fp = std::fopen(fileName.c_str(), "rb")) {
    return std::unique_ptr<StdFile>(new StdFile(fp));
  }

  return nullptr;
}

StdFile::StdFile(FILE *fp)
  : ResourceStream(tell_size(fp)),
    _file(fp) {
}

StdFile::~StdFile() {
  std::fclose(_file);
}

bool StdFile::seek(size_t size) {
  return std::fseek(_file, static_cast<long>(size), SEEK_SET) == 0;
}

size_t StdFile::current_position() {
  auto i = std::ftell(_file);
  if (i < 0)
    return 0;
  return static_cast<size_t>(i);
}

size_t StdFile::read(size_t max_size, void *data) {
  return std::fread(data, max_size, 1, _file);
}
