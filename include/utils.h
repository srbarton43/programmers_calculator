/*
 * common utils header
 */
#ifndef __UTILS_H
#define __UTILS_H

#include "hashtable.h"

#define WORDSIZE 8
#define HT_SIZE 11

int max(int a, int b);

int min(int a, int b);

hashtable_t* setup_ht();

#endif
