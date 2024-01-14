#ifndef DEFINES_H
#define DEFINES_H

#define _GNU_SOURCE

// macro to avoid NULL pointers
#define NONULL(x) ( x == NULL ? "" : x )
// macro to free and initialize to NULL
#define FREENULL(x) if (x) { free(x); x = NULL; }

#define NOCMD      0
#define CMD_REPOS  1
#define CMD_DUPS   2
#define CMD_SEARCH 3
#define CMD_LIST   4
#define CMD_GETUP  5

#define VERSION_STRING "portdbc 1.3"

#endif
