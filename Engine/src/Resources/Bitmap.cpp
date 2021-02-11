#include <Engine/Resources/Bitmap.hpp>

#include <cstdlib>
#include <cstring>
#include <utility>


namespace {
/* 8x8 threshold map (note: the patented pattern dithering algorithm uses 4x4) */
// clang-format off
// @formatter:off
static const unsigned char map[8 * 8] = {
   0,  48,  12,  60,   3,  51,  15,  63,
  32,  16,  44,  28,  35,  19,  47,  31,
   8,  56,   4,  52,  11,  59,   7,  55,
  40,  24,  36,  20,  43,  27,  39,  23,
   2,  50,  14,  62,   1,  49,  13,  61,
  34,  18,  46,  30,  33,  17,  45,  29,
  10,  58,   6,  54,   9,  57,   5,  53,
  42,  26,  38,  22,  41,  25,  37,  21
};
// @formatter:on
// clang-format on

template<typename T>
T tabs(T a) {
  if (a >= 0)
    return a;
  return -a;
}

class NoPaletteBitmap : public e00::resource::Bitmap {
  uint8_t *_data;

public:
  using Bitmap::set_pixel;

  explicit NoPaletteBitmap(std::string name, const e00::Vec2<uint16_t> &size)
    : Bitmap(std::move(name), size, BackingType::MEMORY),
      _data(nullptr) {
    _data = static_cast<uint8_t *>(malloc(static_cast<unsigned long>(size.total()) * sizeof(uint8_t)));
  }

  ~NoPaletteBitmap() override {
    free(_data);
  }

  void clear(uint8_t color_index) override {
    memset(_data, color_index, static_cast<unsigned long>(size().total()) * sizeof(uint8_t));
  }

  void set_pixel(const e00::Vec2<uint16_t> &point, uint8_t color) override {
    if (point.x() <= size().x() && point.y() <= size().y())
      _data[point.y() * size().x() + point.x()] = color;
  }

  uint8_t get_pixel(const e00::Vec2<uint16_t> &point) const override {
    if (point.x() <= size().x() && point.y() <= size().y())
      return _data[point.y() * size().x() + point.x()];
    return 0;
  }
};

class PaletteBitmap : public NoPaletteBitmap {
  e00::Palette<0xFF> _palette;

public:
  PaletteBitmap(std::string name, const e00::Vec2<uint16_t> &size) : NoPaletteBitmap(std::move(name), size) {}

  ~PaletteBitmap() override {}

  const e00::Palette<0xFF> *color_map() const override { return &_palette; }

  void set_color(uint8_t number, e00::Color color) override {
    _palette.colors[number] = color;
  }
};
}// namespace


namespace e00::resource {
std::unique_ptr<Bitmap> Bitmap::CreateMemoryBitmap(std::string name, const Vec2<uint16_t> &size, bool with_color_map) {
  return std::unique_ptr<Bitmap>(with_color_map
                                   ? new PaletteBitmap(std::move(name), size)
                                   : new NoPaletteBitmap(std::move(name), size));
}

Bitmap::Bitmap(std::string name, Vec2<uint16_t> size, BackingType type)
  : Resource(type_id<Bitmap>(), std::move(name)),
    _size(std::move(size)),
    _backed_type(type) {
}

void Bitmap::rect(const Rect<uint16_t> &rect, uint8_t border_color) {
  // +------+
  // |      |
  // |      |
  // +------+

  // from.x, from.y -> to.x, from.y
  // from.x, from.y -> from.x, to.y
  // to.x, to.y -> to.x, from.y
  // to.x, to.y -> from.x, to.y

  line(rect.from(), { rect.to().x(), rect.from().y() }, border_color);
  line(rect.from(), { rect.from().x(), rect.to().y() }, border_color);
  line(rect.to(), { rect.to().x(), rect.from().y() }, border_color);
  line(rect.to(), { rect.from().x(), rect.to().y() }, border_color);
}

void Bitmap::rect(const Rect<uint16_t> &rect, uint8_t border_color, uint8_t fill_color) {
  auto bmp = CreateMemoryBitmap({}, rect.size());
  bmp->clear(fill_color);
  if (border_color != fill_color)
    bmp->rect({ 0, 0, rect.width, rect.height }, border_color);
  blit_bitmap(rect.from(), bmp);
}

void Bitmap::line(const Vec2<uint16_t> &from, const Vec2<uint16_t> &to, uint8_t color) {
  // Bresenham's line algorithm
  const bool steep = (tabs(to.y() - from.y()) > tabs(to.x() - from.x()));
  if (steep) {
    line(from.flip(), to.flip(), color);
    return;
  }

  if (from.x() > to.x()) {
    line(to, from, color);
    return;
  }

  const double dx = to.x() - from.x();
  const double dy = tabs(to.y() - from.y());

  double error = dx / 2.;
  const int ystep = (from.y() < to.y()) ? 1 : -1;
  uint16_t y = from.y();

  const uint16_t maxX = to.x();
  for (uint16_t x = from.x(); x <= maxX; x++) {
    set_pixel({ x, y }, color);
    error -= dy;
    if (error < 0) {
      y += ystep;
      error += dx;
    }
  }
}

void Bitmap::blit_bitmap(const Vec2<uint16_t> &to, const Bitmap *source) {
  for (uint16_t y = 0; y < source->size().y(); y++) {
    for (uint16_t x = 0; x < source->size().x(); x++) {
      const auto color = source->get_pixel({ x, y });
      set_pixel(Vec2<uint16_t>(to.x() + x, to.y() + y), color);
    }
  }
}

void Bitmap::clear(uint8_t color_index) {
  for (uint16_t y = 0; y < size().y(); y++) {
    for (uint16_t x = 0; x < size().x(); x++) {
      set_pixel({ x, y }, color_index);
    }
  }
}
}// namespace e00::resource
