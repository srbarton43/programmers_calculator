#include <stdio.h>

#include "utils.h"

int main(int argc, char *argv[]) {
  int ret;

  prog_data = init_program_data();

#ifdef LIBEDIT
  ret = el_mainloop();
#endif

#ifdef READLINE
  ret = rl_mainloop();
#endif

#ifdef NOHISTORY
  // figure out
#endif

  printf("Thanks for using pcalc :)\n");
  free_program_data(prog_data);
  return ret;
}
