/*
 * hashtable.c - hashtable module
 *
 * see hashtable.h for more information
 *
 * Sam Barton, January 2022
 */

#include "hashtable.h"
#include "hash.h"
#include "mem.h"
#include "set.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hashtable {
  set_t **table;
  int size;
} hashtable_t;

hashtable_t *hashtable_new(const int num_slots) {
  if (num_slots <= 0) {
    return NULL;
  }
  hashtable_t *htable = mem_malloc(sizeof(hashtable_t));
  if (htable == NULL) {
    return NULL;
  } else {
    set_t **array = mem_malloc(num_slots * sizeof(set_t *));
    for (int i = 0; i < num_slots; i++) {
      array[i] = set_new();
    }
    htable->table = array;
    htable->size = num_slots;
    return htable;
  }
}

bool hashtable_insert(hashtable_t *ht, const char *key, void *item) {
  if (ht != NULL && key != NULL && item != NULL) {
    int index = hash_jenkins(key, ht->size); // get index from hashing func
    return set_insert(ht->table[index], key, item); // insert it into set
  } else {
    return false;
  }
}

void *hashtable_find(hashtable_t *ht, const char *key) {
  if (ht != NULL && key != NULL) {
    int index = hash_jenkins(key, ht->size);
    return set_find(ht->table[index], key);
  } else {
    return NULL;
  }
}

void hashtable_print(hashtable_t *ht, FILE *fp,
                     void (*itemprint)(FILE *fp, const char *key, void *item)) {
  if (fp != NULL) {
    if (ht != NULL) {
      set_t *set = NULL;
      for (int i = 0; i < ht->size; i++) {
        set = ht->table[i];
        set_print(set, fp, itemprint);
        printf("\n");
      }
    } else {
      printf("(null)");
    }
  }
}

void hashtable_iterate(hashtable_t *ht, void *arg,
                       void (*itemfunc)(void *arg, const char *key,
                                        void *item)) {
  if (ht != NULL && itemfunc != NULL) {
    set_t *set = NULL;
    for (int i = 0; i < ht->size; i++) {
      set = ht->table[i];
      set_iterate(set, arg, itemfunc);
    }
  }
}

void hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *item)) {
  if (ht != NULL) {
    set_t *set = NULL;
    for (int i = 0; i < ht->size; i++) {
      set = ht->table[i];
      set_delete(set, itemdelete);
    }
    free(ht->table);
    free(ht);
  }
}
