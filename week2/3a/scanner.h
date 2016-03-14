#ifndef _SCANNER_H
#define _SCANNER_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define NUM_OF_WORDS 25
#define MAX_CHAR_NUMS 200

#define MIN(a, b) (a < b)?(a):(b)

typedef struct scanner {
  char words[NUM_OF_WORDS][MAX_CHAR_NUMS];
  int valid;
  int now;
} scanner;

int insert(scanner* sc, const char* new_word);

void free_scanner(scanner* sc);

#endif
