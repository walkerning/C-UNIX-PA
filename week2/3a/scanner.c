#include "scanner.h"

int insert(scanner* sc, const char* word) {
  /* if (sc -> now < sc -> valid) { */
  /*   free(sc -> words[sc -> now]); */
  /* } */

  /* sc -> words[sc -> now] = (char*)malloc((strlen(word) + 1) * sizeof(char)); */
  int repeative = 0;
  int i = 0;
  int len = strlen(word);
  char* regularized_word = sc -> words[sc -> now];

  if (len > MAX_CHAR_NUMS) {
    // 忽略超出长度的单词, 避免缓冲区溢出
    fprintf(stderr, "Word '%s' is too long: length exceed %d. Ignore.\n", word, MAX_CHAR_NUMS);
    return repeative;
  }

  // 去掉结尾的标点符号
  for (; len >= 0; len--) {
    if (isalpha(word[len - 1])) {
      break;
    }
  }

  strncpy(regularized_word, word, len);
  regularized_word[len] = '\0';

  // 转换成小写
  for (i = 0; i < len; i ++) {
    if (isalpha(regularized_word[i])) {
      regularized_word[i] = tolower(regularized_word[i]);
    }
  }

  // 判断是否重复
  for (i = 0; i < sc -> valid; i++) {
    if (i == sc -> now) {
      continue;
    }
    if (strcmp(sc -> words[i], regularized_word) == 0) {
      // repeative
      repeative = 1;
      break;
    }
  }

  sc -> valid = MIN(NUM_OF_WORDS, sc -> valid + 1);
  sc -> now = (sc -> now + 1) % NUM_OF_WORDS;

  return repeative;
}

/* void free_scanner(scanner* sc) { */
/*   int i = 0; */
/*   for (i = 0; i < sc -> valid; i++) { */
/*     free(sc -> words[i]); */
/*   } */
/*   return; */
/* } */
