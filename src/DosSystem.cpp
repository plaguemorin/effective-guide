#include "DosSystem.hpp"

#include <io.h>
#include <dos.h>
#include <cstdio>
#include <cstring>
#include <conio.h>
#include <csignal>
#include <csetjmp>

# include <go32.h>
# include <dpmi.h>
# include <sys/exceptn.h>
# include <sys/nearptr.h>
# include <sys/farptr.h>

constexpr auto TIMERISR = 8;

constexpr uint8_t LOW_BYTE(const uint16_t n) noexcept {
  return n & 0xFFu;
}

constexpr uint8_t HIGH_BYTE(const uint16_t n) noexcept {
  return static_cast<uint8_t>(n >> 8u);
}


namespace {
class InterruptGuard {
  int e;
public:
  InterruptGuard() {
    e = disable();
  }

  ~InterruptGuard() {
    if (e)
      enable();
  }
};

volatile uint32_t _ticks;

/* fixed_timer_handler:
 *  Interrupt handler for the fixed-rate timer driver.
 */
void fixed_timer_handler() {
  ++_ticks;
}

/* set_timer:
 *  Sets the delay time for PIT channel 1 in one-shot mode.
 */
void set_timer(uint16_t time) {
  outportb(0x43, 0x30);
  outportb(0x40, LOW_BYTE(time));
  outportb(0x40, HIGH_BYTE(time));
}


/* set_timer_rate:
 *  Sets the delay time for PIT channel 1 in cycle mode.
 */
void set_timer_rate(uint16_t time) {
  outportb(0x43, 0x34);
  outportb(0x40, LOW_BYTE(time));
  outportb(0x40, HIGH_BYTE(time));
}

/* read_timer:
 *  Reads the elapsed time from PIT channel 1.
 */
uint16_t read_timer() {
  uint16_t x;

  outportb(0x43, 0x00);
  x = inportb(0x40);
  x += inportb(0x40) << 8;

  return (0xFFFF - x + 1) & 0xFFFF;
}

void timer_disable() {
  outportb(0x21, inportb(0x21) | 1);
}

void timer_enable() {
  outportb(0x21, inportb(0x21) & ~1);
}


std::pair<int, int> detectWindows() {
  /* Check Windows 3.X or 9X */
  __dpmi_regs r;
  r.x.ax = 0x1600;
  __dpmi_int(0x2F, &r);

  return std::make_pair(r.h.al, r.h.ah);
}

bool detectWindowsNt() {
  // Check NT
  if (_get_dos_version(1) == 0x0532) {
    return true;
  }

  auto p = getenv("OS");
  return p && strcmp(p, "Windows_NT") == 0;
}

std::pair<bool, int> detectOs2Version() {
  // Check OS2
  __dpmi_regs r;

  r.x.ax = 0x4010;
  __dpmi_int(0x2F, &r);

  /*
   Return:
    AX = 4010h if OS/2 not installed
    AX = 0000h for OS/2 Warp 3.0
    BX = OS/2 version if installed
   */

  if (r.x.ax == 0x4010) {
    return std::make_pair(false, 0);
  }

  if (r.x.ax == 0) {
    return std::make_pair(true, 0);
  }

  return std::make_pair(true, r.x.bx);
}

bool detectDosEmu() {
  // check for Linux DOSEMU
  _farsetsel(_dos_ds);

  char buf[16];
  for (int i = 0; i < 8; i++)
    buf[i] = _farnspeekb(0xFFFF5 + i);

  buf[8] = 0;

  // Check for the BIOS date string "02/25/93" at F000:FFF5 before
  // calling this function. In addition, the segment address of this
  // vector should be F000h (for existing versions of DOSemu,
  // the vector is F000h:0E60h)
  if (!strcmp(buf, "02/25/93")) {
    __dpmi_regs r;
    r.x.ax = 0;
    __dpmi_int(0xE6, &r);
    return r.x.ax == 0xAA55;
  }

  return false;
}

std::pair<bool, bool> detectOpenDOS() {
  __dpmi_regs r;
  r.x.ax = 0x4452;
  __dpmi_int(0x21, &r);

  if ((r.x.ax >= 0x1072) && !(r.x.flags & 1)) {
    /* now check for OpenDOS EMM386.EXE */
    r.x.ax = 0x12FF;
    r.x.bx = 0x0106;
    __dpmi_int(0x2F, &r);

    return std::make_pair(true, (r.x.ax == 0) && (r.x.bx == 0xEDC0));
  }

  return std::make_pair(false, false);
}

SystemType guessSystemType() {
  if (detectDosEmu()) {
    return SystemType::DOSEMU;
  }

  // Detect NT
  if (detectWindowsNt()) {
    return SystemType::WINDOWS_NT;
  }

  // Are we running OS/2 ?
  const auto os2Detect = detectOs2Version();
  if (os2Detect.first) {
    if (os2Detect.second == 0)
      return SystemType::OS2;
    else
      return SystemType::OS2_WARP;
  }

  const auto openDOS = detectOpenDOS();
  if (openDOS.first) {
    if (openDOS.second)
      return SystemType::OPENDOS_EMM386;
    else
      return SystemType::OPENDOS;
  }

  // Check Windows ENH mode
  const auto dpmiInstallCheck = detectWindows();
  switch (dpmiInstallCheck.first) {
    case 3:
      return SystemType::WINDOWS_3X;
    case 4:
      switch (dpmiInstallCheck.second) {
        case 90:
          return SystemType::WINDOWS_ME;
        case 10:
          return SystemType::WINDOWS_98;
        default:
          return SystemType::WINDOWS_95;
      }
  }

  return SystemType::DOS;
}

VirtualDMASpec queryVDS() {
  __dpmi_regs r;
  r.x.ax = 0x8102;
  r.x.bx = 0;
  r.x.dx = 0;
  __dpmi_int(0x4B, &r);

  if (!(r.x.flags & 1)) {
    VirtualDMASpec spec{};

    spec.majorVersion = r.h.ah;
    spec.minorVersion = r.h.al;
    spec.productRevision = r.x.cx;
    spec.productNumber = VirtualDMASpec::UNKNOWN;
    switch (r.x.bx) {
      case 0x0000:
        spec.productNumber = VirtualDMASpec::HPMM;
        break;
      case 0x0001:
        spec.productNumber = VirtualDMASpec::EMM386;
        break;
      case 0x0003:
        spec.productNumber = VirtualDMASpec::WIN386;
        break;
      case 0x0030:
        spec.productNumber = VirtualDMASpec::OS2;
        break;
      case 0x0EDC:
        spec.productNumber = VirtualDMASpec::DRDOS_EMM386;
        break;
      case 0x4560:
        spec.productNumber = VirtualDMASpec::QUALITAS_386MAX;
        break;
      case 0x4D43:
        spec.productNumber = VirtualDMASpec::MEMORY_COMMANDER;
        break;
      case 0x5145:
        spec.productNumber = VirtualDMASpec::QEMM_386;
        break;
      case 0x524D:
        spec.productNumber = VirtualDMASpec::RM386;
        break;
    }

    // Bitfields for VDS flags (DX):
    //
    // Bit(s)  Description     (Table 03219)
    // 0      PC/XT bus (DMA in first megabyte only)
    // 1      physical buffer/remap region in first megabyte
    // 2      automatic remap enabled
    // 3      all memory is physically contiguous
    // 4-15   reserved (zero)

    return spec;
  }

  return {};
}
}

/* signal_handler:
 *  Used to trap various signals, to make sure things get shut down cleanly.
 */
static void signal_handler(int num) {
  raise(num);
}

DOSSystem::DOSSystem()
        : systemType(SystemType::UNSUPPORTED),
          vds(),
          timerOldISR{},
          timerNewISR{} {
  // Setup singleton
  _ticks = 0;

  // Query VDS
  vds = queryVDS();

  // Best guess our system
  systemType = guessSystemType();

  // Check for "stealth" DOS mode in Windows 9X
  if (vds.productNumber == VirtualDMASpec::WIN386) {
    if (systemType == SystemType::DOS || systemType == SystemType::UNSUPPORTED) {
      systemType = SystemType::STEALTH_WINDOWS;
    }
  }

  // install emergency-exit signal handlers
  old_sig_abrt = signal(SIGABRT, &signal_handler);
  old_sig_fpe = signal(SIGFPE, &signal_handler);
  old_sig_ill = signal(SIGILL, &signal_handler);
  old_sig_segv = signal(SIGSEGV, &signal_handler);
  old_sig_term = signal(SIGTERM, &signal_handler);
  old_sig_int = signal(SIGINT, &signal_handler);

#ifdef SIGKILL
  old_sig_kill = signal(SIGKILL, &signal_handler);
#endif

#ifdef SIGQUIT
  old_sig_quit = signal(SIGQUIT, &signal_handler);
#endif

#ifdef SIGTRAP
  old_sig_trap = signal(SIGTRAP, &signal_handler);
#endif

  // Lock our variable, if we need to get called from interrupt routines
  _go32_dpmi_lock_data((void *) &_ticks, sizeof(_ticks));
  _go32_dpmi_lock_code((void *) &fixed_timer_handler, 4096);

  // load the address of the old timer ISR into the timerOldISR structure
  _go32_dpmi_get_protected_mode_interrupt_vector(TIMERISR, &timerOldISR);

  // point timerNewISR to the proper selector:offset for handler function
  timerNewISR.pm_offset = (intptr_t) &fixed_timer_handler;
  timerNewISR.pm_selector = _go32_my_cs();

  // Wrap our new handler
//  _go32_dpmi_allocate_iret_wrapper(&timerNewISR);
  // take control of the timer interrupt
//  _go32_dpmi_set_protected_mode_interrupt_vector(TIMERISR, &timerNewISR);

  // Install new interrupt handler and chain
  _go32_dpmi_chain_protected_mode_interrupt_vector(TIMERISR, &timerNewISR);


  timer_enable();
}

DOSSystem::~DOSSystem() {
  // load the old timer ISR back without the new ISR chained on
  _go32_dpmi_set_protected_mode_interrupt_vector(TIMERISR, &timerOldISR);
  // ** NOT CREATING A WRAPPER **  _go32_dpmi_free_iret_wrapper(&timerNewISR);


  // Remove signal handlers
  signal(SIGABRT, old_sig_abrt);
  signal(SIGFPE, old_sig_fpe);
  signal(SIGILL, old_sig_ill);
  signal(SIGSEGV, old_sig_segv);
  signal(SIGTERM, old_sig_term);
  signal(SIGINT, old_sig_int);

#ifdef SIGKILL
  signal(SIGKILL, old_sig_kill);
#endif

#ifdef SIGQUIT
  signal(SIGQUIT, old_sig_quit);
#endif

#ifdef SIGTRAP
  signal(SIGTRAP, old_sig_trap);
#endif
}

void DOSSystem::yield() const {
  __dpmi_yield();
}

std::chrono::milliseconds DOSSystem::timeSinceStart() const {
  InterruptGuard guard;
  return std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::microseconds(54925 * _ticks));
}


