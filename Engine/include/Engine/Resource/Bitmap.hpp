#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include <Engine/Resource/Resource.hpp>
#include <Engine/Math.hpp>

namespace e00::resource {
class Bitmap : public Resource {
public:
  enum class BackingType {
    HARDWARE, // Only the <OutputScreen> can produce those
    STREAM,
    MEMORY,
  };

private:
  const Vec2I _size;
  const BackingType _backed_type;

protected:
  Bitmap(Vec2I size, BackingType type);

public:
  static std::unique_ptr<Bitmap> CreateMemoryBitmap(const Vec2I& size, bool with_color_map = false);

  ~Bitmap() override = default;

  /**
   * Returns the created size of this bitmap; calls within
   * this should be expected to return valid data
   *
   * @return size of this bitmap
   */
  [[nodiscard]] const Vec2I& size() const { return _size; }

  /**
   * Gets a single pixel in a bitmap.
   * Probably slow
   *
   * @param point valid pixel (less than size())
   * @return color index
   */
  [[nodiscard]] virtual uint8_t get_pixel(const Vec2I& point) const = 0;

  /**
   * Sets a single pixel in a bitmap. Invalid points are silently ignored
   * Probably slow
   *
   * @param point pixel to place color index
   * @param color index
   */
  virtual void set_pixel(const Vec2I& point, uint8_t color) = 0;

  /**
   * Tries to set the pixel at point to color, searching the pallet for
   * nearest match. Default implementation will fail if bitmap has no
   * palette. Calls set_pixel(const Vec2I&, uint8_t) to set the color index
   *
   * @param point
   * @param color
   */
  virtual void set_pixel(const Vec2I& point, const Color& color);

  /**
   * If the current bitmap has a color map, return color map
   *
   * @return the colormap (or null)
   */
  [[nodiscard]] virtual const Palette<0xFF> *color_map() const { return nullptr; }

  virtual void set_color(uint8_t number, Color color) { (void) number; (void) color; };

  // Slow but working methods, consider implementing custom versions of them

  /**
   * Creates a hollow rectangle
   *
   * @param rect position to place the rectangle
   * @param border_color color of the rectangle
   */
  virtual void rect(const RectI& rect, uint8_t border_color);

  virtual void rect(const RectI& rect, uint8_t border_color, uint8_t fill_color);

  virtual void line(const Vec2I& from, const Vec2I& to, uint8_t color);

  void blit_bitmap(const Vec2I& from, const std::unique_ptr<Bitmap>& source) {
    if (source) {
      blit_bitmap(from, *source);
    }
  }

  void blit_bitmap(const Vec2I& from, const std::shared_ptr<Bitmap>& source) {
    if (source) {
      blit_bitmap(from, *source);
    }
  }

  void blit_bitmap(const Vec2I& from, const Bitmap& source) {
    blit_bitmap(from, &source);
  }

  void blit_bitmap(const Vec2I& from, const Bitmap* source);

  virtual void clear(uint8_t color_index);
};
}
