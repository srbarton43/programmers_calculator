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
  printf("  -h, --help                    Show this help message\n");
  printf("  -v, --verbose                 Enable verbose output\n");
  printf("  -b, --binary                  Print in binary format\n");
  printf("  -x, --hex                     Print in hex format\n");
  printf("  -d, --decimal                 Print in decimal format\n");
  printf("  -u, --unsigned                Print in unsigned decimal format\n");
  printf("  -e, --expression <wordsize>   Set the initial wordsize\n");
  printf("  -e, --expression <expr>       Evaluate the expression\n");
}

int main(int argc, char *argv[]) {
  int ret;

  int opt;
  unsigned short verbose_flag = 0;
  unsigned short evaluate_flag = 0;
  char *expression = NULL;
  print_flags_t print_flags = {0};
  
  // Define the long options
  // { name, has_arg, flag, val }
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"verbose", no_argument, 0, 'v'},
      {"binary", no_argument, 0, 'b'},
      {"hex", no_argument, 0, 'x'},
      {"decimal", no_argument, 0, 'd'},
      {"unsigned", no_argument, 0, 'u'},
      {"wordsize", required_argument, 0, 'w'},
      {"expression", required_argument, 0, 'e'},
      {0, 0, 0, 0} // End of array
  };
  
  //  TODO: hacky..should not need for simple expr eval
  memset(&g_prog_data, 0, sizeof(program_data_t));
  g_prog_data.wordsize = DEFAULT_WS;

  // The "hf:" string means:
  // 'h' - no arg
  // 'e:' - requires an argument (denoted by colon)
  while ((opt = getopt_long(argc, argv, "hvbxduw:e:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      return 0;
    case 'v':
      print_flags.VERBOSE = 1;
      break;
    case 'b':
      print_flags.BINARY = 1;
      break;
    case 'x':
      print_flags.HEX = 1;
      break;
    case 'd':
      print_flags.DECIMAL = 1;
      break;
    case 'u':
      print_flags.UNSIGNED = 1;
      break;
    case 'w': {
      char *p = NULL;
      errno = 0;
      long wordsize = strtoul(optarg, &p, 10);
      if (errno == 0 && *p == '\0' && wordsize >= 4 && wordsize <= 128) {
        fprintf(stdout, "Setting wordsize to %d\n", (int)wordsize);
        g_prog_data.wordsize = wordsize;
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
      ret = evaluate_expr(expression, print_flags);
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
  ret = el_mainloop(print_flags);
#endif

#ifdef READLINE
  ret = rl_mainloop();
#endif

#ifdef NOHISTORY
  // figure out
#endif

  printf("Thanks for using pcalc :)\n");
  return ret;
}
