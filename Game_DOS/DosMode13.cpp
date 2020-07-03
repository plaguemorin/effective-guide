
#include "DosMode13.hpp"

#include <go32.h>
#include <dpmi.h>
#include <pc.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <cstring>

namespace {
constexpr auto VGA_BASE_ADDRESS = 0xA0000;

class ScreenVGA_320X200X8 : public e00::sys::OutputScreen {
  e00::Palette<0xFF> _palette;

  uint8_t *get_line(int line) const {
    return reinterpret_cast<uint8_t *>(VGA_BASE_ADDRESS + __djgpp_conventional_base + (320 * line));
  }

public:
  using e00::resource::Bitmap::set_pixel;

  ScreenVGA_320X200X8() : OutputScreen(e00::Vec2I(320, 200), BackingType::HARDWARE) {}

  ~ScreenVGA_320X200X8() override = default;

  uint8_t get_pixel(const e00::Vec2I &point) const override {
    return *(get_line(point.y) + point.x);
  }

  void set_pixel(const e00::Vec2I &point, uint8_t color) override {
    *(get_line(point.y) + point.x) = color;
  }

  std::unique_ptr<e00::resource::Bitmap> convert_to_hardware(const std::unique_ptr<e00::resource::Bitmap> &from) override {
    (void)from;
    return nullptr;
  }

  const e00::Palette<0xFF> *color_map() const override {
    return &_palette;
  }

  void set_color(uint8_t number, e00::Color color) override {
    _palette.colors[number] = color;

    // TODO: convert to 6 bit color
    outportb(0x3C8, number);
    outportb(0x3C9, color.red);
    outportb(0x3C9, color.green);
    outportb(0x3C9, color.blue);
  }
};

inline void vga_vsync() {
  /* wait until any previous retrace has ended */
  do {
  } while (inportb(0x3DA) & 8u);

  /* wait until a new retrace has just begun */
  do {
  } while (!(inportb(0x3DA) & 8u));
}


/* sets a color 0..255 in the palette with rgb values 0,,63 */
inline void vga_setcolor(const uint8_t n, const uint8_t r, const uint8_t g, const uint8_t b) {
  outportb(0x3C8, n);
  outportb(0x3C9, r);
  outportb(0x3C9, g);
  outportb(0x3C9, b);
}

inline uint8_t ditter_2bit(uint8_t value_8bit) {
  if (value_8bit > (0xAA + 0x2A))
    return 0b1001;
  if (value_8bit > (0x55 + 0x2A))
    return 0b0001;
  if (value_8bit >= 0x2B)
    return 0b1000;

  return 0;
}

inline uint8_t ega_color(const uint8_t r, const uint8_t g, const uint8_t b) {
  return static_cast<uint8_t>(
    (ditter_2bit(r) << 2u) | (ditter_2bit(g) << 1u) | (ditter_2bit(b) << 0u));
}

inline void ega_setcolor_direct(const uint8_t pal, const uint8_t r, const uint8_t g, const uint8_t b) {
  inportb(0x03BA);
  outportb(0x03C0, pal & 0x0Fu);
  outportb(0x03C0, ega_color(r, g, b));
}

inline void ega_setcolor_bios(const uint8_t pal, const uint8_t r, const uint8_t g, const uint8_t b) {
  __dpmi_regs regs;
  regs.x.ax = 0x1000;
  regs.h.bh = ega_color(r, g, b);
  regs.h.bl = pal & 0x0Fu;
  __dpmi_int(0x10, &regs);
}

/* set pixel */
inline void vga_pset(const unsigned x, const unsigned y, const uint8_t color) {
  if (x < 320 && y < 200)
    _farpokeb(_dos_ds, 0xA0000 + x + ((y << 8u) + (y << 6u)), color);
}

/* sets 2 pixels at once, color is 2 bytes */
inline void vga_psetw(const unsigned x, const unsigned y, const unsigned short color) {
  if (x < 319 && y < 200)
    _farpokew(_dos_ds, 0xA0000 + x + ((y << 8u) + (y << 6u)), color);
}

/* sets 4 pixels at once, color is 4 bytes */
inline void vga_psetl(const unsigned x, const unsigned y, const unsigned long color) {
  if (x < 317 && y < 200)
    _farpokel(_dos_ds, 0xA0000 + x + ((y << 8u) + (y << 6u)), color);
}
}// namespace

/*

namespace {
class DOSScreen : public Bitmap {
public:
  DOSScreen() : Bitmap({320, 200}) {}

  ~DOSScreen() final = default;

  uint8_t *GetLine(int line) override {
    return reinterpret_cast<uint8_t *>(0xa0000 + __djgpp_conventional_base + (320 * line));
  }

  const uint8_t *GetLine(int line) const override {
    return reinterpret_cast<const uint8_t *>(0xa0000 + __djgpp_conventional_base + (320 * line));
  }
};
}
*/

/*
void DOSMode13::setColor(uint8_t num, uint8_t red, uint8_t green, uint8_t blue) {
    ega_setcolor_bios(num, red, green, blue);
    vga_setcolor(num, red, green, blue);
}
*/

int video_get_current_mode() {
  __dpmi_regs regs{};
  // Get the current video mode
  regs.h.ah = 0xF;
  __dpmi_int(0x10, &regs);
  return regs.h.al;
}

void video_set_current_mode(int mode) {
  __dpmi_regs r;
  r.x.ax = static_cast<unsigned short>(mode);
  __dpmi_int(0x10, &r);
}

VideoInfo video_get_info() {
  VideoInfo video_info{};

  video_info.ega_info = video_get_info_ega();
  video_info.vga_info = video_get_info_vga();

  return video_info;
}

EGA video_get_info_ega() {
  EGA ega_info{};
  ega_info.present = false;
  ega_info.active = false;
  ega_info.color_display = false;

  __dpmi_regs regs;
  // Check for the presence of an EGA and set relevant EGA/VGA parameters
  regs.h.ah = 0x12;// EGA BIOS alternate select. Tech. Ref. p106
  regs.h.bl = 0x10;// return EGA information
  __dpmi_int(0x10, &regs);

  ega_info.present = regs.h.bl != 0x10;
  if (regs.h.bl != 0x10) {
    ega_info.video_ram = regs.h.bl * 64 + 64;
    if (regs.h.bh) {
      ega_info.color_display = true;
      //ega_info.color_display = (regs.h.cl == 3 || regs.h.cl == 9);
    }

    const auto e_byte = _farnspeekb(0x487);
    ega_info.active = !(e_byte & 8u);
  }

  return ega_info;
}

VGA video_get_info_vga() {
  VGA vga_info{};

  __dpmi_regs regs;

  // Check the presence of a VGA
  regs.x.ax = 0x1A00;

  __dpmi_int(0x10, &regs);
  vga_info.present = regs.h.al == 0x1a;
  vga_info.active = vga_info.present && (regs.h.bl >= 7);

  return vga_info;
}

e00::sys::OutputScreen *video_get_screen() {
  return new ScreenVGA_320X200X8();
}
