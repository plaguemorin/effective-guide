
#include "DosMode13.hpp"

#include <go32.h>
#include <dpmi.h>
#include <pc.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <cstring>

namespace {
inline void video_mode_switch(int n) {
  __dpmi_regs r;
  r.x.ax = n;
  __dpmi_int(0x10, &r);
}

inline void vga_vsync() {
  /* wait until any previous retrace has ended */
  do {} while (inportb(0x3DA) & 8u);

  /* wait until a new retrace has just begun */
  do {} while (!(inportb(0x3DA) & 8u));
}


/* sets a color 0..255 in the palette with rgb values 0,,63 */
inline void vga_setcolor(const unsigned char n, const unsigned char r, const unsigned char g, const unsigned char b) {
  outportb(0x3C8, n);
  outportb(0x3C9, r);
  outportb(0x3C9, g);
  outportb(0x3C9, b);
}

inline unsigned char ditter_2bit(unsigned char value_8bit) {
  if (value_8bit > (0xAA + 0x2A))
    return 0b1001;
  if (value_8bit > (0x55 + 0x2A))
    return 0b0001;
  if (value_8bit >= 0x2B)
    return 0b1000;

  return 0;
}

inline unsigned char ega_color(const unsigned char r, const unsigned char g, const unsigned char b) {
  return (ditter_2bit(r) << 2u)
         + (ditter_2bit(g) << 1u)
         + (ditter_2bit(b) << 0u);
}

inline void ega_setcolor_direct(const unsigned char pal, const unsigned char r, const unsigned char g, const unsigned char b) {
  inportb(0x03BA);
  outportb(0x03C0, pal & 0x0Fu);
  outportb(0x03C0, ega_color(r, g, b));
}

inline void ega_setcolor_bios(const unsigned char pal, const unsigned char r, const unsigned char g, const unsigned char b) {
  __dpmi_regs regs;
  regs.x.ax = 0x1000;
  regs.h.bh = ega_color(r, g, b);
  regs.h.bl = pal & 0x0Fu;
  __dpmi_int(0x10, &regs);
}

/* set pixel */
inline void vga_pset(const unsigned x, const unsigned y, const unsigned char color) {
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
}
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

DOSMode13::DOSMode13()
        : modeSwitched(false) {
  __dpmi_regs regs;

  // Get the current video mode
  regs.h.ah = 0xF;
  __dpmi_int(0x10, &regs);
  initialVideoMode = regs.h.al;
  screenCols = regs.h.ah;

  // Check for either VGA/EGA
  checkVgaState();
  checkEgaState();
}

DOSMode13::~DOSMode13() {
  if (modeSwitched) {
    video_mode_switch(initialVideoMode);
  }
  modeSwitched = false;

  printf("VGA Present = %d, Active = %d\nEGA Present = %d, Active = %d\nInitial mode = %d\nVideo Memory = %d\nvideoEcd = %d\n", vgaPresent, vgaActive, egaPresent, egaActive, initialVideoMode,
         videoRam, videoEcd);
}

bool DOSMode13::initialize() {
  // If an EGA or VGA is present then switch
  if (vgaPresent) {
    video_mode_switch(0x13);
  } else if (egaPresent) {
    video_mode_switch(0x0D);
  }
/*
  screen = std::unique_ptr<Bitmap>(new DOSScreen());
  screen->Clear(0);*/

  modeSwitched = true;
  return modeSwitched;
}

void DOSMode13::setColor(uint8_t num, uint8_t red, uint8_t green, uint8_t blue) {
  if (egaActive)
    ega_setcolor_bios(num, red, green, blue);
  if (vgaActive)
    vga_setcolor(num, red, green, blue);
}

void DOSMode13::checkVgaState() {
  __dpmi_regs regs;

  // Check the presence of a VGA
  regs.x.ax = 0x1A00;

  __dpmi_int(0x10, &regs);
  vgaPresent = regs.h.al == 0x1a;
  vgaActive = vgaPresent && (regs.h.bl >= 7);
}

void DOSMode13::checkEgaState() {
  __dpmi_regs regs;
  // Check for the presence of an EGA and set relevant EGA/VGA parameters
  regs.h.ah = 0x12;       // EGA BIOS alternate select. Tech. Ref. p106
  regs.h.bl = 0x10;       // return EGA information
  __dpmi_int(0x10, &regs);

  egaPresent = !(regs.h.bl == 0x10 || vgaPresent);
  egaActive = false;
  videoEcd = false;
  if (egaPresent) {
    videoRam = regs.h.bl * 64 + 64;
    videoColor = regs.h.bh;
    if (videoColor) {
      videoEcd = (regs.h.cl == 3 || regs.h.cl == 9);
    }

    const auto e_byte = _farnspeekb(0x487);
    egaActive = (!vgaActive) && !(e_byte & 8u);
  }
}
