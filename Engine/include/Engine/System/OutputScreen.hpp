#pragma once

#include <memory>

#include <Engine/Math.hpp>
#include <Engine/ResourceSystem/Resources/Bitmap.hpp>

namespace e00::sys {
class OutputScreen : public resource::Bitmap {
protected:
  OutputScreen(const Vec2I &size, BackingType type) : Bitmap(size, type) {}

public:
  ~OutputScreen() override = default;

  /**
   *
   * @param from the source bitmap to convert
   * @return nullptr or a hardware accelerated bitmap version
   */
  virtual std::unique_ptr<resource::Bitmap> convert_to_hardware(const std::unique_ptr<resource::Bitmap>& from) = 0;
};

}
