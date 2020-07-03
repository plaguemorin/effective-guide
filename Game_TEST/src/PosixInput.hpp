#pragma once

#include <termios.h>

class POSIX_Input {
  termios _tty_opts_backup;

public:
  POSIX_Input();

  ~POSIX_Input();

  bool hasInput();

  unsigned short getKey();
};


