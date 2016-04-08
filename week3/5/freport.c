#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include <getopt.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>

#include "freport.h"


/* Info on each directory in the current tree branch, to avoid
   getting stuck in symbolic link loops.  */
static struct dir_id *dir_ids = NULL;
/* Entries allocated in `dir_ids'.  */
static int dir_alloc = 0;
/* Index in `dir_ids' of directory currently being searched.
   This is always the last valid entry.  */
static int dir_curr = -1;
/* (Arbitrary) number of entries to grow `dir_ids' by.  */
#define DIR_ALLOC_STEP 32


/* For long options that have no equivalent short option, use a
   non-character as a pseudo short option, starting with CHAR_MAX + 1.  */
enum {
  NO_ACCESS_OPTION = CHAR_MAX + 1,
  FOLLOW_OPTION
};

static enum {
  IGNORE_DEFAULT,
  IGNORE_MINIMAL
} ignore_mode;

static enum {
  LIST_DEFAULT,
  LIST_VERBOSE,
} list_mode;

static bool follow = false;

static char* PATH = NULL;

static long exceed_limit = 0;

static time_t no_access_limit = 0;

static struct option const long_options[] = {
  {"all", no_argument, NULL, 'a'},
  {"follow", no_argument, NULL, FOLLOW_OPTION},
  {"no-access", required_argument, NULL, NO_ACCESS_OPTION},
  //{"in-use", optional_argument, NULL, 'i'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0}
};

void usage() {
  printf(_("Usage: %s [OPTION]... [PATH]...\n"), PROGRAM_NAME);
  printf(_("Report informations of files under PATH.\n"));
  printf(_("\
  -h, --help            show this help info\n\
  -V, --version         show version info\n\
  \n\
  -a, --all             do not ignore entries starting with .\n\
  -l                    list all informations verbosely\n\
  --follow              follow into subdirectories that are symlinks\n\
  --no-access=TIME      list files that have not been accessed in TIME\n\
  -e, --exceed=SIZE     list files whose size exceed SIZE\n             \n"));
  //-i, --in-use[=USER]   list files currently in use, if USER is specified, only list files currently in use by USER\n\n"));
}

void version() {
  printf(_("%s %s written by %s\n"), PROGRAM_NAME, VERSION, AUTHOR);
}

void issue_loop_warning (char* pathname) {
  //struct stat stbuf_link;
  /* if (lstat(pathname, &stbuf_link) != 0) */
  /*   stbuf_link.st_mode = S_IFREG; */
  /* if (S_ISLNK(stbuf_link.st_mode)) { */
  fprintf(stderr, _("Warning: Symbolic link %s is part of a loop in the directory hierarchy; we have already visited the directory to which it points.\n"), pathname);
    //}
}

bool file_ignored(char* name) {
  return ignore_mode != IGNORE_MINIMAL && name[0] == '.';
}

uid_t name_to_uid(const char* name) {
  struct passwd* pass = getpwnam(name);
  if (pass != NULL) {
    return pass->pw_uid;
  } else {
    return 0;
  }
}

char* uid_to_name(uid_t uid) {
  static struct passwd* pass;
  pass = getpwuid(uid);
  return pass?pass->pw_name:NULL;
}

char* gid_to_name(gid_t gid) {
  static struct group* grp;
  grp = getgrgid(gid);
  return grp?grp->gr_name:NULL;
}

void get_mode_string(struct stat stat_buf, char* mode_string) {
  mode_string[0] = (S_ISDIR(stat_buf.st_mode)) ? 'd' : '-';
  mode_string[1] = (stat_buf.st_mode & S_IRUSR) ? 'r' : '-';
  mode_string[2] = (stat_buf.st_mode & S_IWUSR) ? 'w' : '-';
  mode_string[3] = (stat_buf.st_mode & S_IXUSR) ? 'x' : '-';
  mode_string[4] = (stat_buf.st_mode & S_IRGRP) ? 'r' : '-';
  mode_string[5] = (stat_buf.st_mode & S_IWGRP) ? 'w' : '-';
  mode_string[6] = (stat_buf.st_mode & S_IXGRP) ? 'x' : '-';
  mode_string[7] = (stat_buf.st_mode & S_IROTH) ? 'r' : '-';
  mode_string[8] = (stat_buf.st_mode & S_IWOTH) ? 'w' : '-';
  mode_string[9] = (stat_buf.st_mode & S_IXOTH) ? 'x' : '-';
  mode_string[10] = '\0';
}

void process_file(const char* path, const char* path_, struct stat stat_buf) {
  char mode_string[11];
  time_t t = time(NULL);
  if (t - stat_buf.st_atime < no_access_limit || stat_buf.st_size <= exceed_limit) {
    return;
  }
  if (list_mode == LIST_VERBOSE) {
    // TODO: USE sprintf first, add more verbose informations
    // 加入color option, 如果是目录用蓝色, 再lstat一次如果是软连接用天蓝色
    char buf[sizeof(mode_string) -1 + 1 + MAX_USERNAME_LEN + 1 + MAX_GRP_LEN + 1];
    char* p = buf;
    // mode
    get_mode_string(stat_buf, mode_string);
    sprintf(p, "%s ", mode_string);
    p += strlen(p);

    // user name
    char* username = uid_to_name(stat_buf.st_uid);
    if (username == NULL) {
      // TODO: 改成整个iterate一遍存下需要展示的信息, 最后一起打印信息
      // 容易加入sorting, 计算格式化每个字段所需要的长度等
      // 现在就乱写了个10...
      sprintf(p, "%-10d ", stat_buf.st_uid);
    } else {
      sprintf(p, "%-10s ", username);
    }
    p += strlen(p);

    // group name
    char* grpname = gid_to_name(stat_buf.st_gid);
    if (grpname == NULL) {
      sprintf(p, "%-10d ", stat_buf.st_gid);
    } else {
      sprintf(p, "%-10s ", grpname);
    }
    p += strlen(p);
    printf("%s %s\n", buf, path);
  } else {
    printf("%s", path);
    if (isatty(STDOUT_FILENO)) {
      // multiple files on the same line
      printf("  ");
    } else {
      // one file per line
      printf("\n");
    }
  }
}


time_t parse_time_limit(const char* time_string) {
  // parse literal time string to seconds
  static const char time_suffix[] = {'s', 'm', 'h', 'd'};
  time_t time_sec = atoi(time_string);
  static const int multiplier[] = {1, 60, 3600, 3600 * 24};
  int time_string_len = strlen(time_string);
  int i;
  for (i = 0; i < 4; i ++) {
    if (time_suffix[i] == time_string[time_string_len - 1]) {
      time_sec *= multiplier[i];
      break;
    }
  }
  return time_sec;
}

long parse_size(const char* size_string) {
  static const char size_suffix[] = {'K', 'M', 'G', 'T'};
  long size = atoi(size_string);
  int size_string_len = strlen(size_string);
  int i;
  for (i = 0; i < 4; i++) {
    if (size_suffix[i] == size_string[size_string_len - 1]) {
      size <<= (10 * (i + 1));
      break;
    }
  }
  return size;
}

int process_dir(const char* full_path, const char* path) {
  struct stat stat_buf;
  DIR* d;
  struct dirent *dir_entry;

  bool to_process;
  int i; // loop index
  char real_path[PATH_MAX];
  char* cur_path = NULL;
  char* cur_name = NULL;
  unsigned cur_path_size = 0;
  unsigned file_len;
  int pathlen = strlen(full_path);

  if (stat(path, &stat_buf) != 0) {
    fprintf(stderr, "stat %s: %s\n", path, strerror(errno));
    return 1;
  }
  if (!S_ISDIR(stat_buf.st_mode)) {
    // path is not a valid dir
    return 1;
  }


  if (full_path[pathlen - 1] == '/')
    pathlen = pathlen + 1; /* For '\0'; already have '/'. */
  else
    pathlen = pathlen + 2; /* For '/' and '\0'. */

  realpath(path, real_path);
  if ((d = opendir(real_path)) != NULL) {
    while ((dir_entry = readdir(d)) != NULL) {
      chdir(real_path);
      /* Append this directory entry's name to the path being searched. */
      char* file_name = dir_entry -> d_name;
      if (strcmp(file_name, "..") == 0 || strcmp(file_name, ".") == 0) {
        continue;
      }
      if (file_ignored(file_name)) {
        // ignored file
        continue;
      }

      file_len = pathlen + strlen (file_name);
      if (file_len > cur_path_size)
        {
          while (file_len > cur_path_size)
            cur_path_size += 1024;
          if (cur_path)
            free (cur_path);
          cur_path = malloc (cur_path_size);
          strcpy (cur_path, full_path);
          cur_path[pathlen - 2] = '/';
        }
      cur_name = cur_path + pathlen - 1;
      strcpy (cur_name, file_name);

      if (stat(cur_name, &stat_buf) != 0) {
          fprintf(stderr, "%s: %s\n", cur_path, strerror(errno));
          continue;
      }

      process_file(cur_path, cur_name, stat_buf);

      // recursive into sub directories
      if ((DT_LNK == dir_entry -> d_type && follow == true && S_ISDIR(stat_buf.st_mode))
          || DT_DIR == dir_entry -> d_type) {
        to_process = true;
        for (i = 0; i <= dir_curr; i++)
          if (stat_buf.st_ino == dir_ids[i].ino &&
              stat_buf.st_dev == dir_ids[i].dev)
            {
              issue_loop_warning(cur_path);
              to_process = false;
              break;
            }

        if (dir_alloc <= ++dir_curr)
          {
            dir_alloc += DIR_ALLOC_STEP;
            dir_ids = (struct dir_id *)
              realloc ((char *) dir_ids, dir_alloc * sizeof (struct dir_id));
          }
        dir_ids[dir_curr].ino = stat_buf.st_ino;
        dir_ids[dir_curr].dev = stat_buf.st_dev;
        if (to_process) {
          process_dir(cur_path, cur_name);
        }
      }
    }
    closedir(d);
  }

  return 0;
}


int main(int argc, char** argv) {
  int c = -1;
  int option_index = -1;
  while ((c = getopt_long(argc, argv, "alhVie:", long_options, &option_index)) != -1) {
    switch (c) {
    case 'a':
      ignore_mode = IGNORE_MINIMAL;
      break;
    case 'l':
      list_mode = LIST_VERBOSE;
      break;
    case 'h':
      usage();
      exit(0);
    case 'V':
      version();
      exit(0);
    /* case 'i': */
    /*   // parse user */
    /*   break; */
    case 'e':
      // parse size
      exceed_limit = parse_size(optarg);
      break;
    case NO_ACCESS_OPTION:
      // parse time
      no_access_limit = parse_time_limit(optarg);
      break;
    case FOLLOW_OPTION:
      follow = true;
      break;
    default:
      break;
    }
  }
  // optional positional argument
  if (argv[optind] == NULL) {
    PATH = ".";
  } else {
    PATH = argv[optind];
  }

  int status = process_dir(PATH, PATH);
  return status;
}
