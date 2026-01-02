#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef LIBEDIT
#include <histedit.h>
#endif

#ifdef READLINE
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#endif

#include "number.h"
#include "parser.tab.h"
#include "utils.h"

#ifdef LIBEDIT
/* This holds all the state for our line editor */
EditLine *el;
#endif

program_data_t *prog_data;
// Use _zero_ from number.c instead
static const status_t _emptystatus_ = {0};
void yylex_destroy(void);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse(number_t **output, status_t *status, u64 *arg);

/* static functions for libedit */
#ifdef LIBEDIT
static char *prompt(EditLine *e) { return PROMPT; }
static void quit_message(int signal) {
  printf("\ninterrupt (type \"quit\" to exit)\n");
  el_set(el, EL_REFRESH);
}
#endif

int evaluate_expr(const char *expr) {
  int ret;
  number_t *number = 0;
  number_alloc(&number);
  status_t status = _emptystatus_;
  u64 arg = 0;

  yy_scan_string(expr);
  ret = yyparse(&number, &status, &arg);
  if (ret != 0) {
    printf("syntax error\n");
  } else {
    if (n_UNSIGNED_OVERFLOW(number))
      printf("Error: There was a unsigned overflow...the resulting number "
             "was larger than the wordsize!\n");
    else {
      printf("  =\n");
      number_print(number);
      if (n_SIGNED_OVERFLOW(number))
        printf("Warning: There was a signed overflow...the integer value "
               "might be inaccurate!\n");
    }
  }
  number_destroy(number);
  return ret;
}

#ifdef LIBEDIT
int el_mainloop() {
  /* This holds the info for our history */
  History *myhistory;

  /* Temp variables */
  int count;
  const char *line;
  int keepreading = 1;
  HistEvent ev;
  number_t *number = 0;
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
      /* Clean up our memory */
      buffer = yy_scan_string(line);
      ret = yyparse(&number, &status, &arg);
      if (!status.EMPTY)
        history(myhistory, &ev, H_ENTER, line);
      // printf("ret=%d\n", ret);
      yy_delete_buffer(buffer);
      if (ret != 0) {
        printf("syntax error\n");
      } else if (status.EMPTY) {
        // pass
      } else if (status.QUIT_SIG) {
        break;
      } else if (status.UNDEF_VAR) {
        printf("Error...cannot use an undefined variable\n");
      } else if (status.POISON) {
        printf("Error...\n");
      } else if (status.WSIZE_PR) {
        printf("The current wordsize is %d\n", prog_data->wordsize);
      } else if (status.WSIZE_CHG) {
        int new_wsize = arg;
        if (new_wsize < 4 || new_wsize > 128) {
          printf("unsupported wordsize: %d\n", new_wsize);
        } else {
          printf("changed wordsize to %d\n", new_wsize);
          prog_data->wordsize = new_wsize;
        }
      } else if (status.VAR_ASSN) {
        // var assignment
        vars_set_num(prog_data, (char)arg, number); // TODO: don't copy number, use ptr
        printf("%c\n  = \n", (char)arg);
        number_print(number);
      } else {
        if (n_UNSIGNED_OVERFLOW(number))
          printf("Error: There was a unsigned overflow...the resulting number "
                 "was larger than the wordsize!\n");
        else {
          printf("  =\n");
          number_print(number);
          if (n_SIGNED_OVERFLOW(number))
            printf("Warning: There was a signed overflow...the integer value "
                   "might be inaccurate!\n");
          if (! status.ACCESS_VAR) {
            number_destroy(number);
          }
        }
      }
    } else {
      printf("\n");
      break;
    }
    status = _emptystatus_;
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

  p_data->wordsize = DEFAULT_WS;
  p_data->status = _emptystatus_;

  return p_data;
}

void print_program_data(program_data_t *p_data) {
  printf("\n***********************\n");
  printf("Variables\n");
  for (int i = 0; i < VAR_NUM; i++) {
    printf("%c = ", 'a' + i);
    number_t *var_val = 0;
    if (SUCCESS == vars_get_num(&var_val, p_data, i+'a')) {
      number_print(var_val);
    } else {
      printf("\n");
    }
  }
  printf("wordsize: %d\n", p_data->wordsize);
}

void free_program_data(program_data_t *p_data) { 
  for (int i = 0; i < VAR_NUM; i++) {
    number_destroy(p_data->vars[i]);
  }

  free(p_data); 
}

int vars_get_num(number_t **out, program_data_t *p_data, char var) {
  int idx = var - 'a';
  *out = p_data->vars[idx];
  if (*out == NULL) {
    return ERROR;
  }
  return SUCCESS;
}

void vars_set_num(program_data_t *p_data, char var, number_t *num) {
  int idx = var - 'a';
  if (!p_data->vars[idx]) {
    number_alloc(&p_data->vars[idx]);
  }
  copy_number(p_data->vars[idx], num, p_data->wordsize);
}
