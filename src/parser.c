#include <stdlib.h>
#include <stdio.h>

#include "../include/number.h"
#include "../include/file.h"

void parse_line(char* line) {
  if (line == NULL) {
    fprintf(stderr, "parse_line: line is null");
    return;
  }
  

  free(line);
  
}
