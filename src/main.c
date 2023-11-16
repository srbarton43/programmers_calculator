
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "number.h"
#include "stack.h"
#include "parser.h"

int main(int argc, char *argv[]) {
  printf("Hello World\n");
//     // printf("Query? ");
//     // fflush(stdout);
//     // char* line = NULL;
//     // while ((line = file_readLine(stdin)) != NULL) {
//     //   printf("%s\n", line);
//     //   free(line);
//     //   printf("Query? ");
//     // }
//     // printf("\n");
//   printf("%lu", sizeof(unsigned long));
//   // number_t* n1 = new_number(0, "1111");
//   // number_t* n2 = new_number(0, "1001");
//   number_t* n3 = new_number(0, "111111110");
//   // number_t* n4 = new_number(1, "69");
//   // number_t* n5 = new_number(2, "food");
//   // number_print(n1);
//   // number_print(n2);
//   number_print(n3);
//   number_t* rshifted = rshift(n3, 2);
//   printf("rshift n3\n");
//   number_print(rshifted);
//   number_t* lshifted = lshift(rshifted, 2);
//   printf("Post LSHIFT\n");
//   number_print(lshifted);
//   // number_print(n4);
//   // number_print(n5);
//   // delete_number(n1);
//   // delete_number(n2);
//   delete_number(n3);
//   delete_number(lshifted);
//   delete_number(rshifted);
//   // delete_number(n4);
//   // delete_number(n5);
//   stack* stk = new_stack();
//   char* s1 = "abc";
//   char* s2 = "def";
//   char buf[32];
//   push(stk, s1);
//   push(stk, s2);
//   strcpy(buf, (char*)pop(stk));
//   printf("%s\n", buf);
//   strcpy(buf, (char*)pop(stk));
//   printf("%s\n", buf);
    char* line = malloc(100*sizeof(char));
    strcpy(line, "- >> --- abc");
    parse_line(line);
    printf("foo\n");
    return 0;
    
}
