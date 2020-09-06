#pragma once

#include <Engine/ResourceSystem/Resources/Bitmap.hpp>
#include <Engine/Stream/Stream.hpp>

std::unique_ptr<e00::resource::Bitmap> LoadBMP(const std::unique_ptr<e00::Stream>& stream);

