#include "PosixInput.hpp"

#include <unistd.h>
#include <poll.h>

POSIX_Input::POSIX_Input() {

  // Back up current TTY settings
  tcgetattr(STDIN_FILENO, &_tty_opts_backup);

  termios raw = _tty_opts_backup;

  /* input modes - clear indicated ones giving: no break, no CR to NL,
     no parity check, no strip char, no start/stop output (sic) control */
  raw.c_iflag = 0;

  /* output modes - clear giving: no post processing such as NL to CR+NL */
  raw.c_oflag = 0;

  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);

  /* local modes - clear giving: echoing off, canonical off (no erase with
     backspace, ^U,...), no extended functions, no signal chars (^Z,^C) */
  raw.c_lflag &= static_cast<unsigned int>(~(ECHO | ICANON | IEXTEN | ISIG));

  /* control chars - set return condition: min number of bytes and timer */
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0; /* immediate - anything */

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

POSIX_Input::~POSIX_Input() {
  // Restore previous TTY settings
  tcsetattr(STDIN_FILENO, TCSANOW, &_tty_opts_backup);
}

bool POSIX_Input::hasInput() {
  // Do we have any input on STDIN ?
  pollfd input{};
  input.fd = STDIN_FILENO;
  input.events = POLLIN;

  return poll(&input, 1, 0) >= 1;
}

unsigned short POSIX_Input::getKey() {
  unsigned short buffer;
  if (read(STDIN_FILENO, &buffer, 1) == 1) {
    return buffer;
  }

  return 0;
}
