#include <linux/types.h>

struct info {
  char id[11];
  char department[5];
  __be16 age;
  char name[128];
};
