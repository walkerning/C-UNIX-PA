#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <dirent.h>
#include <endian.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

  FILE* stream = fdopen(ifd, "r");
  pid_t pid;
  char* tmpdir_name = tmpnam(NULL);
  // FIXME: my mkdtemp seems to fail because of my own environment issue...
  if (mkdir(tmpdir_name, 0770) != 0) {
    perror("Create temp directory fail");
    exit(1);
  }

  while (getline(&line, &size, stream) != -1) {
    while ((pid = fork()) < 0) {
      perror("fork failed, will retry after 2 seconds...");
      sleep(2);
    }
    if (pid == 0) {
      // subprocess
      // open tmp output file
      chdir(tmpdir_name);
      char ofname_buf[16];
      int ofd_tmp;
      pid_t sub_pid = getpid();
      sprintf(ofname_buf, "tmp_%d", sub_pid);
      if ((ofd_tmp = open(ofname_buf, O_WRONLY|O_CREAT|O_TRUNC, 0640)) == -1) {
        fprintf(stderr, "in subprocess %d: open tmp file %s failed.\n", sub_pid, ofname_buf);
        exit(1);
      }
      info_ins = parse_csv_line(line);
      if (write(ofd_tmp, &info_ins, sizeof(struct info)) != sizeof(struct info)) {
        fprintf(stderr, "in subprocess %d: Warning: write to binary fail. The binary may corrupt!", sub_pid);
      }
      free(line);
      close(ofd_tmp);
      exit(0);
    }
    line = NULL;
  }

  // wait for all child processes to terminate
  while ((pid = waitpid(-1, NULL, 0)) != -1) {
    if (errno == ECHILD) {
      break;
    }
  }
  DIR* d;
  struct dirent *dir_entry;
  int tmp_fd;
  char cwd[PATH_MAX];

  // open output file
  if ((ofd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0640)) == -1) {
    perror("Open binary file fail");
    exit(1);
  }

  getcwd(cwd, PATH_MAX);

  if ((d = opendir(tmpdir_name)) != NULL) {
    chdir(tmpdir_name);
    while ((dir_entry = readdir(d)) != NULL) {
      if (dir_entry -> d_type == DT_REG) {
        if ((tmp_fd = open(dir_entry -> d_name, O_RDONLY)) == -1) {
          fprintf(stderr, "In concatenating step: open tmp file %s fail: %s. Skip!\n", dir_entry -> d_name, strerror(errno));
        } else if (read(tmp_fd, &info_ins, sizeof(struct info)) != sizeof(struct info)) {
          fprintf(stderr, "In concatenating step: tmp file %s is corrupted. Skip!\n", dir_entry -> d_name);
        } else if (write(ofd, &info_ins, sizeof(struct info)) != sizeof(struct info)) {
          fprintf(stderr, "In concatenating step: write to binary fail. The binary may corrupt!");
        }
        if (unlink(dir_entry -> d_name) != 0) {
          fprintf(stderr, "Remove temp file %s fail: %s\n", dir_entry -> d_name, strerror(errno));
        }
      }
    }
  }
  chdir(cwd);
  if (rmdir(tmpdir_name) != 0) {
    fprintf(stderr, "Remove temp dir %s fail: %s\n", tmpdir_name, strerror(errno));
  }
  close(ifd);
  close(ofd);
  return 0;
}
