
#include <iterator>
#include <vector>

#include "Loaders/BitmapLoader.hpp"

#include "LODEPNG.H"
/*

namespace {
class LodePngBitmap : public Bitmap {
  unsigned char* _image;
  size_t _image_size;

public:
  LodePngBitmap(const LodePNGState& state, unsigned char* image, int w, int h)
          : Bitmap({w, h}), _image(image) {
    _image_size = lodepng_get_raw_size(w, h, &state.info_raw);
  }

  ~LodePngBitmap() override {
    delete _image;
  }

  uint8_t *GetLine(int line) override {
    return _image + (line * GetSize().x);
  }

  const uint8_t *GetLine(int line) const override {
    return _image + (line * GetSize().x);
  }
};
}

std::pair<std::error_code, std::unique_ptr<Bitmap>> PNGLoader::LoadBitmap(std::istream& istream) {
  LodePngBitmap *output = nullptr;

  {
    // get its size:
    std::streampos fileSize;
    istream.seekg(0, std::ios::end);
    fileSize = istream.tellg();
    istream.seekg(0, std::ios::beg);

    std::vector<uint8_t> input;
    input.reserve(fileSize);

    // read the data:
    input.insert(input.begin(),
                 std::istream_iterator<uint8_t>(istream),
                 std::istream_iterator<uint8_t>());

    LodePNGState state;
    lodepng_state_init(&state);
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;

    unsigned char *image = nullptr;
    unsigned width, height;

    auto error = lodepng_decode(&image, &width, &height, &state, input.data(), input.size());
    if (error) {
      return std::make_pair(std::make_error_code(std::errc::operation_not_supported), nullptr);
    }

    output = new LodePngBitmap(state, image, (int) width, (int) height);

    lodepng_state_cleanup(&state);
  }

  return std::make_pair(std::error_code(),
                        std::unique_ptr<LodePngBitmap>(output));
}
*/
