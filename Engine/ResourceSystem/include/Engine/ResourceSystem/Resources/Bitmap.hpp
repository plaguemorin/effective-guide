#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include <Engine/ResourceSystem/Resource.hpp>

#include <Engine/Math/Vec2.hpp>
#include <Engine/Math/Rect.hpp>
#include <Engine/Math/Palette.hpp>
#include <Engine/Math/Color.hpp>

namespace e00::resource {
class Bitmap : public Resource {
public:
  enum class BackingType {
    HARDWARE,// Only the <OutputScreen> can produce those
    STREAM,
    MEMORY,
  };

private:
  const Vec2<int> _size;
  const BackingType _backed_type;

protected:
  Bitmap(Vec2<int> size, BackingType type);

public:
  static std::unique_ptr<Bitmap> CreateMemoryBitmap(const Vec2<int> &size, bool with_color_map = false);

  ~Bitmap() override = default;

  /**
   * Returns the created size of this bitmap; calls within
   * this should be expected to return valid data
   *
   * @return size of this bitmap
   */
  [[nodiscard]] const Vec2<int> &size() const { return _size; }

  /**
   * Gets a single pixel in a bitmap.
   * Probably slow
   *
   * @param point valid pixel (less than size())
   * @return color index
   */
  [[nodiscard]] virtual uint8_t get_pixel(const Vec2<int> &point) const = 0;

  /**
   * Sets a single pixel in a bitmap. Invalid points are silently ignored
   * Probably slow
   *
   * @param point pixel to place color index
   * @param color index
   */
  virtual void set_pixel(const Vec2<int> &point, uint8_t color) = 0;

  /**
   * Tries to set the pixel at point to color, searching the pallet for
   * nearest match. Default implementation will fail if bitmap has no
   * palette. Calls set_pixel(const Vec2I&, uint8_t) to set the pixel
   *
   * @param point
   * @param color
   */
  virtual void set_pixel(const Vec2<int> &point, const Color &color) {
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
    (void) number;
    (void) color;
  };

  // Slow but working methods, consider implementing custom versions of them

  /**
   * Creates a hollow rectangle
   *
   * @param rect position to place the rectangle
   * @param border_color color of the rectangle
   */
  virtual void rect(const Rect<int> &rect, uint8_t border_color);

  virtual void rect(const Rect<int> &rect, uint8_t border_color, uint8_t fill_color);

  virtual void line(const Vec2<int> &from, const Vec2<int> &to, uint8_t color);

  void blit_bitmap(const Vec2<int> &from, const std::unique_ptr<Bitmap> &source) {
    if (source) {
      blit_bitmap(from, *source);
    }
  }

  void blit_bitmap(const Vec2<int> &from, const std::shared_ptr<Bitmap> &source) {
    if (source) {
      blit_bitmap(from, *source);
    }
  }

  void blit_bitmap(const Vec2<int> &from, const Bitmap &source) {
    blit_bitmap(from, &source);
  }

  virtual void blit_bitmap(const Vec2<int> &from, const Bitmap *source);

  virtual void clear(uint8_t color_index);
};
}// namespace e00::resource
