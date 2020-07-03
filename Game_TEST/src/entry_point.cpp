#include <cstdio>

#include <Engine.hpp>
#include "SystemImpl.hpp"

int main(int argc, char **argv) {
  SystemImpl system;

  if (argc > 0) {
    printf("ARGV[0]: %s\r\n", argv[0]);
  }

  Engine engine(&system);

  while (engine.running()) {
    system.process_input();

    engine.update();

    system.clear_input();
  }

  printf("\r\n---\r\nQUITTING\r\n---\r\n");
  printf("E00 was running for = %ld milliseconds\r\n",
         system.time_since_start().count());
  return 0;
}
