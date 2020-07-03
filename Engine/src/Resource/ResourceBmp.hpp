#pragma once

#include <Engine/Resource/Bitmap.hpp>
#include <Engine/ResourceStream.hpp>

std::unique_ptr<e00::resource::Bitmap> LoadBMP(const std::unique_ptr<e00::ResourceStream>& stream);

