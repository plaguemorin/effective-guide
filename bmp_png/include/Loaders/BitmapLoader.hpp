#pragma once

#include <memory>
#include <system_error>
#include <iostream>

#include <Engine/Bitmap.hpp>

class PNGLoader : public BitmapLoader {
public:
  PNGLoader() = default;
  ~PNGLoader() = default;

};
