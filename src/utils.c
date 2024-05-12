#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LIBEDIT
#include <histedit.h>
#endif

#ifdef READLINE
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#endif

#include "number.h"
#include "utils.h"
#include "parser.tab.h"

#ifdef LIBEDIT
/* This holds all the state for our line editor */
EditLine *el;
#endif

program_data_t *prog_data;
static const number_t _emptynumber_ = {0};
static const status_t _emptystatus_ = {0};
void yylex_destroy(void);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse (number_t *output, status_t *status, u64 *arg);

/* static functions for libedit */
#ifdef LIBEDIT
static char *prompt(EditLine *e) { return PROMPT; }
static void quit_message(int signal) {
  printf("\ninterrupt (type \"quit\" to exit)\n%s", PROMPT);
  fflush(stdout);
}
#endif

#ifdef LIBEDIT
int el_mainloop() {
  /* This holds the info for our history */
  History *myhistory;

  /* Temp variables */
  int count;
  const char *line;
  int keepreading = 1;
  HistEvent ev;
  number_t number = _emptynumber_;
  status_t status = _emptystatus_;
  u64 arg = 0;
  YY_BUFFER_STATE buffer;
  int ret;

  /* Initialize the EditLine state to use our prompt function and
  emacs style editing. */

  el = el_init("pc", stdin, stdout, stderr);
  el_set(el, EL_PROMPT, &prompt);
  el_set(el, EL_EDITOR, "emacs");
  signal(SIGINT, quit_message);

  /* Initialize the history */
  myhistory = history_init();
  if (myhistory == 0) {
    fprintf(stderr, "history could not be initialized\n");
    return ERROR;
  }

  /* Set the size of the history */
  history(myhistory, &ev, H_SETSIZE, 800);

  /* This sets up the call back functions for history functionality */
  el_set(el, EL_HIST, history, myhistory);

  while (keepreading) {
    /* count is the number of characters read.
       line is a const char* of our command line with the tailing \n */
    line = el_gets(el, &count);

    /* In order to use our history we have to explicitly add commands
    to the history */
    if (count > 0) {
      history(myhistory, &ev, H_ENTER, line);
      /* Clean up our memory */
      buffer = yy_scan_string(line);
      ret = yyparse(&number, &status, &arg);
      // printf("ret=%d\n", ret);
      yy_delete_buffer(buffer);
      if (status.QUIT_SIG) {
        break;
      } else if (status.POISON) {
        printf("Error...\n");
      } else if (status.NUM_BUF_OF) {
        printf("Error: More numbers than the program can handle\n");
      } else if (status.WSIZE_PR) {
        printf("The current wordsize is %d\n", prog_data->wordsize);
      } else if (status.WSIZE_CHG) {
        int new_wsize = arg;
        if (new_wsize < 4 || new_wsize > 64) {
          printf("unsupported wordsize: %d\n", new_wsize);
        } else {
          printf("changed wordsize to %d\n", new_wsize);
          prog_data->wordsize = new_wsize;
        }
      } else if (status.VAR_ASSN) {
        // var assignment
        vars_set_num(prog_data, (char)arg, &number);
        printf("%c\n  = \n", (char)arg);
        number_print(&number);
      } else {
        if (number.metadata.UNSIGNED_OVERFLOW)
          printf("Error: There was a unsigned overflow...the resulting number "
                 "was larger than the wordsize!\n");
        else {
          printf("  =\n");
          number_print(&number);
          if (number.metadata.SIGNED_OVERFLOW)
            printf("Warning: There was a signed overflow...the integer value "
                   "might be inaccurate!\n");
        }
      }
    } else {
      printf("\n");
      break;
    }
    status = _emptystatus_;
    number = _emptynumber_;
  }
  history_end(myhistory);
  el_end(el);
  yylex_destroy();
  return 0;
}
#endif

#ifdef READLINE
int rl_mainloop(void) {
  char *line;
  char *w_newline;
  int slen;
  while ((line = readline(PROMPT)) != NULL) {
    slen = strlen(line);
    if (slen > 0) {
      add_history(line);
    }
    w_newline = malloc((slen + 2) * sizeof(char));
    strcpy(w_newline, line);
    strcat(w_newline, "\n");
    buffer = yy_scan_string(w_newline);
    yyparse();
    yy_delete_buffer(buffer);
    free(w_newline);
    free(line);
  }
  rl_clear_history();
}
#endif

program_data_t *init_program_data(void) {
  program_data_t *p_data = malloc(sizeof(program_data_t));

  for (int i = 0; i < VAR_NUM; i++) {
    copy_number(&p_data->vars[i], &_zero_, 4);
  }
  p_data->nbuf_ptr = 0;
  p_data->wordsize = DEFAULT_WS;
  p_data->status = _emptystatus_;
  memset(&p_data->numbers_buf, 0, MAX_NUMBERS_COUNT * sizeof(number_t));

  return p_data;
}

void print_program_data(program_data_t *p_data) {
  printf("\n***********************\n");
  printf("Variables\n");
  for (int i = 0; i < VAR_NUM; i++) {
    printf("%c = ", 'a' + i);
    number_print(&p_data->vars[i]);
  }
  printf("wordsize: %d\n", p_data->wordsize);
}

void free_program_data(program_data_t *p_data) { free(p_data); }

number_t vars_get_num(program_data_t *prog_data, char var) {
  int idx = var - 'a';
  number_t num = prog_data->vars[idx];
  return num;
}

void vars_set_num(program_data_t *prog_data, char var, number_t *num) {
  int idx = var - 'a';
  copy_number(&prog_data->vars[idx], num, num->wordsize);
}
