#pragma once

#include <cstdio>
#include <string>
#include <memory>
#include <Engine/ResourceStream.hpp>

class StdFile : public e00::ResourceStream {
  FILE *const _file;

public:
  using e00::ResourceStream::read;

  static std::unique_ptr<StdFile> CreateFromFilename(const std::string &fileName);

  explicit StdFile(FILE *fp);

  ~StdFile() override;

  bool seek(size_t size) override;

  size_t current_position() override;

  size_t read(size_t max_size, void *data) override;
};
