#pragma once

#include <Engine/ResourceSystem/Resource.hpp>

namespace e00::resource {
class Sprite : public Resource {
public:
  Sprite();
  ~Sprite() override;
};
}
