
#include <stdio.h>
#include <stdlib.h>
#include "../include/file.h"
#include "../include/number.h"

int main(int argc, char *argv[]) {
  printf("Hello World\n");
    // printf("Query? ");
    // fflush(stdout);
    // char* line = NULL;
    // while ((line = file_readLine(stdin)) != NULL) {
    //   printf("%s\n", line);
    //   free(line);
    //   printf("Query? ");
    // }
    // printf("\n");
  printf("%lu", sizeof(unsigned long));
  number_t* n1 = new_number(0, "1111");
  number_t* n2 = new_number(0, "1001");
  number_t* n3 = new_number(0, "111111110");
  number_t* n4 = new_number(1, "69");
  number_t* n5 = new_number(2, "food");
  number_print(n1);
  number_print(n2);
  number_print(n3);
  number_print(n4);
  number_print(n5);
  delete_number(n1);
  delete_number(n2);
  delete_number(n3);
  delete_number(n4);
  delete_number(n5);
  return 0;
}
