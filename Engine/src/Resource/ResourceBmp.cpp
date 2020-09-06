#include "ResourceBmp.hpp"

std::unique_ptr<e00::resource::Bitmap> LoadBMP(const std::unique_ptr<e00::Stream>& stream) {
  uint16_t header;
  stream->read(header);

  if (header != 0x4D42) {
    return nullptr;
  }

  uint32_t file_size;
  stream->read(file_size);
  uint32_t _reserved;
  stream->read(_reserved);
  uint32_t data_offset;
  stream->read(data_offset);
  uint32_t header_size;
  stream->read(header_size);
  int32_t height, width;
  stream->read(width);
  stream->read(height);
  uint16_t planes;
  stream->read(planes);
  uint16_t bit_count;
  stream->read(bit_count);
  uint32_t compression{0};
  stream->read(compression);
  uint32_t size_image{0};
  stream->read(size_image);
  int32_t x_pixels_per_meter{0};
  stream->read(x_pixels_per_meter);
  int32_t y_pixels_per_meter{0};
  stream->read(y_pixels_per_meter);
  uint32_t colors_used{0};
  stream->read(colors_used);
  uint32_t colors_important{0};
  stream->read(colors_important);

  if (bit_count != 8) {
    // Only support 8bpp images
    return nullptr;
  }

  // Pixel data
  stream->seek(data_offset);
  auto bmp = e00::resource::Bitmap::CreateMemoryBitmap(e00::Vec2<int>(width, height), false);

  for (auto y = 0; y < height; y++) {
    for (auto x = 0; x < width; x++) {
      uint8_t color_idx{0};

      switch (bit_count) {
        case 8:
          if (stream->read(color_idx)) {
            bmp->set_pixel(e00::Vec2<int>(x, y), color_idx);
          }
          break;
      }
    }
  }

  return bmp;
}
