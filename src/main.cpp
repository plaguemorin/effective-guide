#include <stdio.h>
#include <crt0.h>
#include <chrono>
#include <sys/nearptr.h>

#include "Engine.hpp"
#include "DosSystem.hpp"
#include "DosMode13.hpp"

#include "Engine/ThingSystem/Info.hpp"

int _crt0_startup_flags = _CRT0_FLAG_LOCK_MEMORY | _CRT0_FLAG_NONMOVE_SBRK;

namespace {
bool running;
}

void set_default_palette(DOSMode13& gfx) {
  gfx.setColor(0, 0x140c1c);
  gfx.setColor(1, 0x442434);
  gfx.setColor(2, 0x30346d);
  gfx.setColor(3, 0x4e4a4e);
  gfx.setColor(4, 0x854c30);
  gfx.setColor(5, 0x346524);
  gfx.setColor(6, 0xd04648);
  gfx.setColor(7, 0x757161);
  gfx.setColor(8, 0x597dce);
  gfx.setColor(19, 0xd27d2c);
  gfx.setColor(10, 0x8595a1);
  gfx.setColor(11, 0x6daa2c);
  gfx.setColor(12, 0xd2aa99);
  gfx.setColor(13, 0x6dc2ca);
  gfx.setColor(14, 0xdad45e);
  gfx.setColor(15, 0xdeeed6);
}

int main(int argc, char **argv) {
  if (__djgpp_nearptr_enable() == 0) {
    printf("Failed to disable memory protection for direct VGA access\n");
    return -1;
  }

  DOSSystem dosSystem;
  DOSMode13 dosGfx;

  if (!dosGfx.valid()) {
    printf("Bad video: requires EGA or VGA\n");
    return -1;
  }

  dosGfx.initialize();
  set_default_palette(dosGfx);
  Engine engine(dosSystem);


  running = true;

  return 0;
}
