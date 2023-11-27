/*
 * set.c - set module
 *
 * see set.h for more information
 *
 * Sam Barton, January 2022
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "mem.h"

typedef struct setpair {
  char* key; // key string
  void* item; // item
  struct setpair* next;
} setpair_t;

typedef struct set {
  struct setpair* head;
} set_t;

static setpair_t* setpair_new(const char* key, void* item);

set_t*
set_new(void)
{
  set_t* set = mem_malloc(sizeof(set_t) );
  if (set == NULL) {
    return NULL; // error allocating memory
  } else {
    // initialize set stuff
    set->head = NULL;
    return set;
  }
}

bool
set_insert(set_t* set, const char* key, void* item)
{
  if (set != NULL && item != NULL && key != NULL) {
    // allocate new setpair
      // must make sure it is not a duplicate
      setpair_t* pairP = set->head; // pointer to set pair that starts at head
      while (pairP != NULL) {
        if (strcmp(key, pairP->key) == 0) {
          return false; // key alread exists in set
        }
        pairP = pairP->next;
      }
      // insert unique key into set
      setpair_t* new = setpair_new(key, item);
      new->next = set->head;
      set->head = new;
      return true;
  }
  return false;
}

/* allocate and initialize a setpair */
static setpair_t*
setpair_new(const char* key, void* item)
{
  setpair_t* pair = malloc(sizeof(setpair_t));
  if (pair == NULL) {
    return NULL; // because of error allocating memory for setpair
  }
  // try to allocate memory for key, free pair if fails
  char* buffer = malloc((strlen(key) + 1) * sizeof(char));
  strcpy(buffer, key);
  pair->key = buffer;
  if (pair->key == NULL) {
    free(pair);
    return NULL;
  } else {
    // instantiate data for the setpair if memory allocation works
    pair->item = item;
    pair->next = NULL;
    return pair;
  }
}

void*
set_find(set_t* set, const char* key)
{
  if (set == NULL || key == NULL) {
    return NULL;
  } else {
    for(setpair_t* pairP = set->head; pairP != NULL; pairP = pairP->next) {
      if (strcmp(pairP->key, key) == 0) {
        return pairP->item;
      }
    }
    return NULL;
  }
}

void
set_print(set_t* set, FILE* fp,
    void (*itemprint)(FILE* fp, const char* key, void* item) )
{
  if (fp != NULL) {
    if (set != NULL) {
      fputc('{', fp);
      for (setpair_t* pair = set->head; pair != NULL; pair = pair->next) {
        if (itemprint != NULL) {
          (*itemprint)(fp, pair->key, pair->item); // prints out one pair
          if(pair->next != NULL) {
            fputc(',', fp); // comma if not last pair
          }
        }
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

void
set_iterate(set_t* set, void* arg,
    void (*itemfunc)(void* arg, const char* key, void* item) ) 
{
  if (set != NULL && itemfunc != NULL) {
    for (setpair_t* pairP = set->head; pairP != NULL; pairP = pairP->next) {
        (*itemfunc)(arg, pairP->key, pairP->item);
    }
  }
}

void
set_delete(set_t* set, void (*itemdelete)(void* item) )
{
  if (set != NULL) {
    for (setpair_t* pairP = set->head; pairP != NULL; ) {
      if(itemdelete != NULL) {
        (*itemdelete)(pairP->item);
      }
      setpair_t* next = pairP->next;
      free(pairP->key);
      free(pairP);
      pairP = next;
    }
    free(set);
  }
}
