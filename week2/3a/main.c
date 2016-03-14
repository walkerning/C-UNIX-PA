#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

char word[MAX_CHAR_NUMS];

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "需要一个参数:input文件名\n");
    exit(1);
  }

  FILE* fp = fopen(argv[1], "r");
  char c;
  scanner sc;
  int repeative;
  while (1) {
    c = fscanf(fp, "%s", word); /* got one word from the file */
    if (c == EOF) {
      break;
    }

    repeative = insert(&sc, word);
    if (repeative) {
      printf("重复的词语: %s\n", word);
    }
  }
  return 0;
}
