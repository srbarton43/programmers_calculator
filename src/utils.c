#include "utils.h"
#include "hashtable.h"
#include "number.h"

int max (int a, int b) {
  if (a >= b)
    return a;
  return b;
}

int min (int a, int b) {
  if (a <= b) 
    return a;
  return b;
}

hashtable_t* setup_ht() {
  hashtable_t* ht = hashtable_new(HT_SIZE);
  hashtable_insert(ht, "0", copy_number(_zero_, 0));
  hashtable_insert(ht, "1", copy_number(_one_, 0));
  return ht;
}