#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <ctype.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include "number.h"
#include "file.h"

#define MAX_TOK_SIZE 12

#define OPERATORS_REGEX "[*+\\-=/]|<<|>>"
#define NUMBER_REGEX "0x[a-f\\d]+|\\d+"
#define HEX_REGEX "0x[a-f\\d]+"

typedef enum tok_type {OPERATOR, VARIABLE, NUMBER} tok_type;

static int token_number = 0;

typedef struct o_token {
  int id;
  tok_type type_e;
  int isValid;
  char* raw_token;
  char cleaned_token[MAX_TOK_SIZE];
} token_t;

static void token_print (token_t* token) {
  printf("token id: %d\n", token->id);
  printf("isValid: %d\n", token->isValid);
  printf("raw token: %s\n", token->raw_token);
  printf("cleaned token: %s\n", token->cleaned_token);
  printf("type: %d\n", token->type_e);
  printf("foo\n");
}

static char* isMatch (char* raw_token, char* regex) {
  int value;

  pcre2_code *re;
  PCRE2_SPTR pattern = (PCRE2_SPTR) regex;     /* PCRE2_SPTR is a pointer to unsigned code units of */
  PCRE2_SPTR subject= (PCRE2_SPTR) raw_token;     /* the appropriate width (8, 16, or 32 bits). */
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
    return NULL;
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
      // case PCRE2_ERROR_NOMATCH: printf("No Match\n"); break;
      /*
      Handle other special cases if you like
      */
      default: printf("Matching error %d\n", rc); break;
    }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /* data and the compiled pattern. */
    return NULL;
  }

  PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);

  if (rc == 0)
    printf("ovector was not big enough for all the captured substrings\n");
  
  // prints output vector from regex match
  PCRE2_SPTR substring_start = subject + ovector[0];
  size_t substring_length = ovector[1] - ovector[0];
  printf("%2d: %.*s\n", 0, (int)substring_length, (char *)substring_start);
  // buffer for the cleaned token ... must be freed by caller
  char* buffer = malloc((MAX_TOK_SIZE+1)*sizeof(char)); 
  sprintf(buffer, "%.*s", (int)substring_length, (char *)substring_start);
  if (strlen(buffer) != strlen(raw_token)) { // we chopped characters off of raw
    // printf("not all of token matched\n");
    return NULL;
  }

  return buffer;
}



static void token_classify(token_t* token) {
  char* buffer;
  buffer = isMatch(token->raw_token, OPERATORS_REGEX);
  if (buffer != NULL) {
    token->type_e = OPERATOR;
    goto  matched;
  } 
  buffer = isMatch(token->raw_token, NUMBER_REGEX);
  if (buffer != NULL) {
    token->type_e = NUMBER;
    goto matched;
  }

  // if we have gotten here then the token did not match any patterns
  printf("%s: invalid token\n", token->raw_token);
  return;

  matched: 
  strcpy(token->cleaned_token, buffer); // copy buffer into cleaned token
  token->isValid = 1;
  free(buffer);
}

token_t** tokenize (char* line) {
  char* slow = line;                                        // slow pointer
  char* fast;                                               // fast pointer
  token_t** tokens = malloc(strlen(line)*sizeof(token_t));  // token array
  int token_p = 0;                                          // token pointer
  // simple tokenize on blankspace
  while (*slow != '\0') {
    if (isspace(*slow)) {
      slow++;
    } else {
      fast = slow+1;
      while (*fast != '\0' && !isspace(*fast)) {
        fast++;
      }
      if (isspace(*fast)) *fast = '\0'; // if fast pointer is not at end, null-terminate the string
      tokens[token_p] = malloc(sizeof(token_t));
      tokens[token_p]->id = token_number;
      tokens[token_p]->raw_token = slow;
      tokens[token_p]->isValid = 0;
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