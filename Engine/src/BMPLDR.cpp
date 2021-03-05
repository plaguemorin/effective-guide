#include "BMPLDR.hpp"

namespace {
struct BmpLoaderErrorCode : std::error_category {
  const char *name() const noexcept override { return "engine"; }
  std::string message(int ev) const override {
    switch (static_cast<e00::impl::BmpLoaderErrorCode>(ev)) {
      case e00::impl::BmpLoaderErrorCode::not_valid_bmp: return "not a valid bmp";
    }
    return {};
  }
};

const BmpLoaderErrorCode bmp_loader_error_code{};
}

namespace e00::impl {

std::error_code make_error_code(BmpLoaderErrorCode e) {
  return std::error_code(static_cast<int>(e), bmp_loader_error_code);
}

std::vector<std::unique_ptr<resource::Bitmap>> load_bmp_from_stream(const std::unique_ptr<Stream>& stream) {
  // BMPs have 54 bytes of header... so we need at least that
  if (stream->stream_size() < 54) {
    return {};
  }

  uint8_t signature[2];
  stream->read(signature);

  // Check signature
  if (signature[0] != 'B' || signature[1] != 'M') {
    return {};
  }

  // Check file size
  uint16_t fileSize;
  stream->read(fileSize);

  uint16_t reserved1;
  stream->read(reserved1);

  uint16_t dataOffset;
  stream->read(dataOffset);


}
}
