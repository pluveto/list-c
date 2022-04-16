#include "util.h"

void
abort_on_error (int errno)
{
  if (errno)
    {
      const char *err = strerror (errno);
      fprintf (stderr, "error: %s\n", err);
      abort ();
    }
}
