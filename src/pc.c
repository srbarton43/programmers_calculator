#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_EXPR_LEN 128

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
  unsigned short verbose_flag = 0;
  unsigned short evaluate_flag = 0;
  char *expression = NULL;
  
  // Define the long options
  // { name, has_arg, flag, val }
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"verbose", no_argument, 0, 'v'},
      {"wordsize", required_argument, 0, 'w'},
      {"expression", required_argument, 0, 'e'},
      {0, 0, 0, 0} // End of array
  };
  
  //  TODO: hacky..should not need for simple expr eval
  prog_data = init_program_data();

  // The "hf:" string means:
  // 'h' - no arg
  // 'e:' - requires an argument (denoted by colon)
  while ((opt = getopt_long(argc, argv, "hvw:e:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      return 0;
    case 'v':
      verbose_flag = 1;
      printf("Verbose mode enabled.\n");
      break;
    case 'w': {
      char *p = NULL;
      errno = 0;
      long wordsize = strtoul(optarg, &p, 10);
      if (errno == 0 && *p == '\0' && wordsize >= 4 && wordsize <= 128) {
        fprintf(stdout, "Setting wordsize to %d\n", (int)wordsize);
        // set wordsize
      } else {
        fprintf(stderr, "Wordsize must be a valid integer between 4 and 128 inclusive\n");
      }
      break;
              }
    case 'e':
      printf("evaluate\n");
      evaluate_flag = 1;
      size_t len = strnlen(optarg, MAX_EXPR_LEN) + 1;
      expression = malloc(len);
      strncpy(expression, optarg, len);
      break;
    case '?':
      return 1;
    default:
      abort();
    }
  }
  
  if (evaluate_flag) {
      printf("evaluating %s\n", expression);
      ret = evaluate_expr(expression);
      free(expression);
      exit(ret);
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
