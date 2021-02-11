#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include <Engine/Resource.hpp>
#include <Engine/Resources/ResourcePtr.hpp>

#include <Engine/Math/Vec2.hpp>
#include <Engine/Math/Rect.hpp>
#include <Engine/Math/Palette.hpp>
#include <Engine/Math/Color.hpp>
#include <utility>

namespace e00::resource {
class WindowedBitmap;

class Bitmap : public Resource {
public:
  enum class BackingType {
    HARDWARE,// Only the <OutputScreen> can produce those
    STREAM,
    MEMORY,
    WINDOWED,// Windowed
  };

private:
  const Vec2<uint16_t> _size;
  const BackingType _backed_type;

protected:
  Bitmap(std::string name, Vec2<uint16_t> size, BackingType type);

  [[nodiscard]] bool within_range(const Vec2<uint16_t> &point) const noexcept {
    return (point.x() <= _size.x()) && (point.y() <= _size.y());
  }

public:
  static std::unique_ptr<Bitmap> CreateMemoryBitmap(std::string name, const Vec2<uint16_t> &size, bool with_color_map = false);

  ~Bitmap() override = default;

  WindowedBitmap make_window(const Vec2<uint16_t> &from, const Vec2<uint16_t> &size);

  WindowedBitmap make_window(const Rect<uint16_t> &window);

  /**
   * Returns the created size of this bitmap; calls within
   * this should be expected to return valid data
   *
   * @return size of this bitmap
   */
  [[nodiscard]] const decltype(_size) &size() const { return _size; }

  /**
   * Returns the backing contained_type of this bitmap
   *
   * @return backing contained_type
   */
  [[nodiscard]] BackingType backing_type() const { return _backed_type; }

  /**
   * Gets a single pixel in a bitmap.
   * Probably slow
   *
   * @param point valid pixel (less than size())
   * @return color index
   */
  [[nodiscard]] virtual uint8_t get_pixel(const Vec2<uint16_t> &point) const = 0;

  /**
   * Sets a single pixel in a bitmap. Invalid points are silently ignored
   * Probably slow
   *
   * @param point pixel to place color index
   * @param color index
   */
  virtual void set_pixel(const Vec2<uint16_t> &point, uint8_t color) = 0;

  /**
   * Tries to set the pixel at point to color, searching the pallet for
   * nearest match. Default implementation will fail if bitmap has no
   * palette. Calls set_pixel(const Vec2I&, uint8_t) to set the pixel
   *
   * @param point
   * @param color
   */
  virtual void set_pixel(const Vec2<uint16_t> &point, const Color &color) {
    if (const auto palette = color_map()) {
      const auto closest = palette->find_closest_color(color);
      set_pixel(point, closest);
    }
  }

  /**
   * If the current bitmap has a color map, return color map
   *
   * @return the colormap (or null)
   */
  [[nodiscard]] virtual const Palette<0xFF> *color_map() const { return nullptr; }

  /**
   * Sets a color in the color map
   *
   * @param number color index
   * @param color the color to set this color index to
   */
  virtual void set_color(uint8_t number, Color color) {
    (void)number;
    (void)color;
  };

  // Slow but working methods, consider implementing custom versions of them

  /**
   * Creates a hollow rectangle
   *
   * @param rect position to place the rectangle
   * @param border_color color of the rectangle
   */
  virtual void rect(const Rect<uint16_t> &rect, uint8_t border_color);

  virtual void rect(const Rect<uint16_t> &rect, uint8_t border_color, uint8_t fill_color);

  virtual void line(const Vec2<uint16_t> &from, const Vec2<uint16_t> &to, uint8_t color);

  void blit_bitmap(const Vec2<uint16_t> &to, const std::unique_ptr<Bitmap> &source) {
    if (source) {
      blit_bitmap(to, *source);
    }
  }

  void blit_bitmap(const Vec2<uint16_t> &to, const resource::ResourcePtr<Bitmap> &source) {
    if (source) {
      blit_bitmap(to, *source);
    }
  }

  void blit_bitmap(const Vec2<uint16_t> &to, const Bitmap &source) {
    blit_bitmap(to, &source);
  }

  virtual void blit_bitmap(const Vec2<uint16_t> &to, const Bitmap *source);

  virtual void clear(uint8_t color_index);
};

/**
 * No copy bitmap into another bitmap- the user must ensure the reference outlives this projection
 *
 */
class WindowedBitmap : public Bitmap {
  friend class Bitmap;

  Bitmap &_source;
  const Vec2<uint16_t> _offset;

protected:
  WindowedBitmap(Bitmap &source, Vec2<uint16_t> offset, Vec2<uint16_t> size)
    : Bitmap(source.name(), std::move(size), BackingType::WINDOWED),
      _source(source),
      _offset(std::move(offset)) {}

public:
  WindowedBitmap(const WindowedBitmap &other)
    : Bitmap(other._source.name(), other.size(), BackingType::WINDOWED),
      _source(other._source),
      _offset(other._offset) {}

  ~WindowedBitmap() override = default;

  [[nodiscard]] uint8_t get_pixel(const Vec2<uint16_t> &point) const override {
    return _source.get_pixel(_offset + point);
  }

  void set_pixel(const Vec2<uint16_t> &point, uint8_t color) override {
    _source.set_pixel(_offset + point, color);
  }

  void set_pixel(const Vec2<uint16_t> &point, const Color &color) override {
    _source.set_pixel(_offset + point, color);
  }

  void rect(const Rect<uint16_t> &rect, uint8_t border_color) override {
    _source.rect(rect + _offset, border_color);
  }

  void rect(const Rect<uint16_t> &rect, uint8_t border_color, uint8_t fill_color) override {
    _source.rect(rect + _offset, border_color, fill_color);
  }

  void line(const Vec2<uint16_t> &from, const Vec2<uint16_t> &to, uint8_t color) override {
    _source.line(_offset + from, _offset + to, color);
  }

  void blit_bitmap(const Vec2<uint16_t> &from, const Bitmap *source) override {
    _source.blit_bitmap(_offset + from, source);
  }

  void clear(uint8_t color_index) override {
    _source.rect(
      Rect<uint16_t>(_offset, size()),
      color_index);
  }
};

inline WindowedBitmap Bitmap::make_window(const Rect<uint16_t> &window) {
  return make_window(window.from(), window.size());
}

inline WindowedBitmap Bitmap::make_window(const Vec2<uint16_t> &from, const Vec2<uint16_t> &size) {
  // Are the dimensions legal ?
  if (!within_range(from)) {
    return make_window(Vec2<uint16_t>(0, 0), size);
  }

  if (!within_range(from + size)) {
    return make_window(from, this->size());
  }

  // Are we a windowed bitmap; if so don't window a window - apply the transformation
  if (_backed_type == Bitmap::BackingType::WINDOWED) {
    auto real = static_cast<WindowedBitmap*>(this);
    return WindowedBitmap(real->_source, from + real->_offset, size);
  }

  return WindowedBitmap(*this, from, size);
}

}// namespace e00::resource
