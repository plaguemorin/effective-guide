#pragma once

#include <string>
#include <cstdio>

#include <Engine/Stream.hpp>

class ResourceStreamSystemImpl : public ResourceStream {
  FILE *_file;

public:
  using ResourceStream::read;

  explicit ResourceStreamSystemImpl(const std::string& filename);

  ~ResourceStreamSystemImpl() override;

  size_t stream_size() override;

  bool seek(size_t size) override;

  size_t read(size_t max_size, void *data) override;
};


