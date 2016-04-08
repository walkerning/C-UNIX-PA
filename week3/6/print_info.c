#include <stdio.h>
#include <endian.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "info.h"

int main(int argc, char** argv) {
  int fd;
  struct info info_ins;
  if (argc != 2) {
    fprintf(stderr, "Need 1 argument: binary file path.\n");
    exit(1);
  }

  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    perror("Open binary file fail");
    exit(1);
  } else {
    while (read(fd, &info_ins, sizeof(struct info)) == sizeof(struct info)) {
      printf("%*s %*s %*d %s\n", 11, info_ins.id, 5, info_ins.department, 3, be16toh(info_ins.age), info_ins.name);
    }
    close(fd);
  }
  return 0;
}
