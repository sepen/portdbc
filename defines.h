#include <stdio.h>

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

#define VERSION_STRING "portdbc 1.2 by Jose V Beneyto, <sepen@crux.nu>"

size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}