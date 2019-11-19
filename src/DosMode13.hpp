#pragma once

#include "Engine.hpp"

class DOSMode13 /*: public GFX */{
  bool modeSwitched;
  unsigned char initialVideoMode;
  unsigned char screenCols;
  bool vgaPresent{};
  bool vgaActive{};
  bool egaPresent{};
  bool egaActive{};
  int videoRam{};
  bool videoColor{};
  bool videoEcd{}; // Ega driving an color display ?

  //std::unique_ptr<Bitmap> screen;

  void checkVgaState();

  void checkEgaState();

public:
  DOSMode13();

  ~DOSMode13();

  //const std::unique_ptr<Bitmap>& GetScreen() override { return screen; }

  bool valid() const { return vgaActive || egaActive; }

  bool initialize();

  void setColor(uint8_t num, uint8_t red, uint8_t green, uint8_t blue);

  void setColor(uint8_t num, uint32_t color_rgb) {
    setColor(
            num,
            (color_rgb & 0xFF0000u) >> 16u,
            (color_rgb & 0xFF00u) >> 8u,
            color_rgb & 0xFFu
    );
  }


};

