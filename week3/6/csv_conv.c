#include <stdio.h>
#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "info.h"

char* find_first_non_space(char* field) {
  int i = 0;
  while (field[i] != '\0') {
    if (field[i] != ' ' && field[i] != '\t') {
      break;
    }
    i ++;
  }
  return field + i;
}

struct info parse_csv_line(char* line) {
  struct info info_ins;
  char* p;
  // id
  p = strtok(line, ",\n");
  strncpy(info_ins.id, find_first_non_space(p), 10);
  info_ins.id[10] = '\0';

  // name
  p = strtok(NULL, ",\n");
  strncpy(info_ins.name, find_first_non_space(p), 127);
  info_ins.name[127] = '\0';

  // department
  p = strtok(NULL, ",\n");
  strncpy(info_ins.department, find_first_non_space(p), 4);
  info_ins.department[4] = '\0';

  // age
  p = strtok(NULL, ",\n");
  uint16_t num = atoi(find_first_non_space(p));
  info_ins.age = htobe16(num);
  return info_ins;
}

int main(int argc, char** argv) {
  int ifd;
  int ofd;
  size_t size;
  struct info info_ins;
  char* line = NULL;
  if (argc != 3) {
    fprintf(stderr, "Need 2 argument: csv file path and binary file path.\n");
    exit(1);
  }

  if ((ifd = open(argv[1], O_RDONLY)) == -1) {
    perror("Open csv file fail");
    exit(1);
  }

  if ((ofd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0640)) == -1) {
    perror("Open binary file fail");
    exit(1);
  }

  FILE* stream = fdopen(ifd, "r");
  while (getline(&line, &size, stream) != -1) {
    info_ins = parse_csv_line(line);
    if (write(ofd, &info_ins, sizeof(struct info)) != sizeof(struct info)) {
      fprintf(stderr, "Warning: write to binary fail. The binary may corrupt!");
    }
    free(line);
    line = NULL;
  }

  close(ifd);
  close(ofd);
  return 0;
}
