#include <stdio.h>
#include <stdlib.h>
#if defined(LIBEDIT)
#include <histedit.h>
#elif defined(READLINE)
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "utils.h"
#include "parser.tab.h"

void yylex_destroy(void);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

// extern hashtable_t* ht;
// extern int global_wordsize;
program_data_t *prog_data;

#if defined(LIBEDIT)
/* To print out the prompt you need to use a function.  This could be
made to do something special, but I opt to just have a static prompt.
*/
char *prompt(EditLine *e) { return ">>> "; }
#endif

int main(int argc, char *argv[]) {

#ifdef LIBEDIT
  /* This holds all the state for our line editor */
  EditLine *el;

  /* This holds the info for our history */
  History *myhistory;

  /* Temp variables */
  int count;
  const char *line;
  int keepreading = 1;
  HistEvent ev;

  /* Initialize the EditLine state to use our prompt function and
  emacs style editing. */

  el = el_init(argv[0], stdin, stdout, stderr);
  el_set(el, EL_PROMPT, &prompt);
  el_set(el, EL_EDITOR, "emacs");

  /* Initialize the history */
  myhistory = history_init();
  if (myhistory == 0) {
    fprintf(stderr, "history could not be initialized\n");
    return 1;
  }

  /* Set the size of the history */
  history(myhistory, &ev, H_SETSIZE, 800);

  /* This sets up the call back functions for history functionality */
  el_set(el, EL_HIST, history, myhistory);
#endif

  prog_data = init_program_data();
  YY_BUFFER_STATE buffer;
#ifdef LIBEDIT
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
      yyparse();
      yy_delete_buffer(buffer);
    } else {
      break;
    }
  }
  history_end(myhistory);
  el_end(el);
#endif
#ifdef READLINE
  char *line;
  char *w_newline;
  int slen;
  while((line = readline(">>> ")) != NULL) {
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
#endif
  rl_clear_history();
  yylex_destroy();
  printf("\nThanks for using pcalc :)\n");
  free_program_data(prog_data);
  return 0;
}
