#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void print_usage(char *prog_name) {
  printf("Usage: %s [options]\n", prog_name);
  printf("Options:\n");
  printf("  -h, --help                Show this help message\n");
  printf("  -v, --verbose             Enable verbose output\n");
  printf("  -e, --expression <expr>   Evaluate the expression\n");
}

int main(int argc, char *argv[]) {
  int ret;

  int opt;
  int verbose_flag = 0;

  // Define the long options
  // { name, has_arg, flag, val }
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"verbose", no_argument, 0, 'v'},
      {"expression", required_argument, 0, 'e'},
      {0, 0, 0, 0} // End of array
  };
  
  //  TODO: hacky..should not need for simple expr eval
  prog_data = init_program_data();

  // The "hf:" string means:
  // 'h' - no arg
  // 'e:' - requires an argument (denoted by colon)
  while ((opt = getopt_long(argc, argv, "hve:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      return 0;
    case 'v':
      verbose_flag = 1;
      printf("Verbose mode enabled.\n");
      break;
    case 'e':
      printf("evaluating %s\n", optarg);
      ret = evaluate_expr(optarg);
      return 0;
    case '?':
      // getopt_long already prints an error message
      return 1;
    default:
      abort();
    }
  }

  // Handle any remaining non-option arguments (like positional filenames)
  // TODO: maybe sue these as input and keep running program?
  //       kind of like `bc -f` option
//  if (optind < argc) {
//    printf("Positional arguments: ");
//    while (optind < argc)
//      printf("%s ", argv[optind++]);
//    printf("\n");
//  }

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
