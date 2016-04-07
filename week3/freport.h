#ifndef _FREPORT_H
#define _FREPORT_H

// meta information
#define VERSION "0.1"
#define PROGRAM_NAME "freport"
#define AUTHOR "Foxfi"


// for l10n
#include <locale.h>
#include <libintl.h>
#define _(S) gettext(S)

#define GETOPT_HELP_OPTION_DECL                 \
  "help", no_argument, NULL, 'h'
#define GETOPT_VERSION_OPTION_DECL              \
  "version", no_argument, NULL, 'V'

/* dir_id is used to support loop detection in find.c
 */
#include <sys/types.h>
struct dir_id {
  ino_t ino;
  dev_t dev;
};

#define true 1
#define false 0
#define bool int

#define MAX_USERNAME_LEN  32
#define MAX_GRP_LEN 32
#define MAX_FILE_NAME_LENGTH 25
#endif
