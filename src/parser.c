#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#include <regex.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include "number.h"
#include "file.h"

#define OPERATORS_REGEX "[*+\\-=/]|<<|>>"

typedef enum tok_type {OPERATOR, VARIABLE, NUMBER} tok_type;

static int token_number = 0;

typedef struct o_token {
  int id;
  tok_type type_e;
  char* raw_token;
  char cleaned_token[12];
} token_t;

void token_print(token_t* token) {
  printf("token id: %d\n", token->id);
  printf("raw token: %s\n", token->raw_token);
  printf("cleaned token: %s\n", token->cleaned_token);
  printf("type: %d\n", token->type_e);
  printf("foo\n");
}

void token_classify(token_t* token) {
  char* raw_token = token->raw_token;
  token->id = token_number;

  int value;

  pcre2_code *re;
  PCRE2_SPTR pattern = (PCRE2_SPTR) OPERATORS_REGEX;     /* PCRE2_SPTR is a pointer to unsigned code units of */
  PCRE2_SPTR subject= (PCRE2_SPTR) raw_token;     /* the appropriate width (8, 16, or 32 bits). */
  PCRE2_SPTR name_table;
  int errornumber;
  PCRE2_SIZE erroroffset;
  
  re = pcre2_compile(
    pattern,               /* the pattern */
    PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
    0,                     /* default options */
    &errornumber,          /* for error number */
    &erroroffset,          /* for error offset */
    NULL);                 /* use default compile context */

/* Compilation failed: print the error message and exit. */

  if (re == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
    fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
      buffer);
    exit(1);
  }

  /* Using this function ensures that the block is exactly the right size for
the number of capturing parentheses in the pattern. */

  pcre2_match_data *match_data;
  int rc;
  size_t subject_length = strlen((char*)raw_token);

  match_data = pcre2_match_data_create_from_pattern(re, NULL);

  rc = pcre2_match(
    re,                   /* the compiled pattern */
    subject,              /* the subject string */
    subject_length,       /* the length of the subject */
    0,                    /* start at offset 0 in the subject */
    0,                    /* default options */
    match_data,           /* block for storing the result */
    NULL);                /* use default match context */

  printf("\nREGEX Pattern: %s\n", (char*)pattern);
  printf("token: %s\n", raw_token);
  
  if (rc < 0) {
    switch(rc)
    {
      case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
      /*
      Handle other special cases if you like
      */
      default: printf("Matching error %d\n", rc); break;
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /* data and the compiled pattern. */
    token->type_e = VARIABLE;
    return;
  }

  PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
  printf("Match succeeded at offset %d\n", (int)ovector[0]);

  token->type_e = OPERATOR;

  if (rc == 0)
    printf("ovector was not big enough for all the captured substrings\n");
  
  // prints output vector from regex match
  PCRE2_SPTR substring_start = subject + ovector[0];
  size_t substring_length = ovector[1] - ovector[0];
  printf("ovector: %s\n", (char*)ovector);
  printf("%2d: %.*s\n", 0, (int)substring_length, (char *)substring_start);
  sprintf(token->cleaned_token, "%.*s\n", (int)substring_length, (char *)substring_start);

}

token_t** tokenize (char* line) {
  char* slow = line;
  char* fast;
  token_t** tokens = malloc(strlen(line)*sizeof(token_t));
  int token_p = 0;
  while (*slow != '\0') {
    if (*slow == ' ') {
      slow++;
    } else {
      fast = slow+1;
      while (*fast != '\0' && *fast != ' ') {
        fast++;
      }
      if (*fast == ' ') *fast = '\0';
      tokens[token_p] = malloc(sizeof(token_t));
      tokens[token_p]->raw_token = slow;
      token_classify(tokens[token_p]);
      token_number++;
      token_p++;
      slow = fast+1;
    } 
  }
  return tokens;
}
void parse_line(char* line) {
  if (line == NULL) {
    fprintf(stderr, "parse_line: line is null");
    return;
  }

  token_t** tokens = tokenize(line);
  for (int i = 0; tokens[i] != NULL; i++) {
    token_print(tokens[i]);
  }
  free(line);
  
}