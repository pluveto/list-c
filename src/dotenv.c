#include "dotenv.h"

int
set_env (const char *key, const char *value)
{
  return setenv (key, value, 1);
}

const char *
get_env (const char *key, const char *def)
{
  const char *value = getenv (key);
  if (!value)
    {
      value = def;
    }
  return value;
}

int
load_dotenv ()
{
  const char *path = get_env ("DOTENV_PATH", ".env");
  return load_env (path);
}

int
load_env (const char *path)
{
  FILE *fp = fopen (path, "r");
  if (!fp)
    {
      return -1;
    }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline (&line, &len, fp)) != -1)
    {
      char *key = strtok (line, "=");
      char *value = strtok (NULL, "\n");
      set_env (key, value);
    }

  fclose (fp);
  return 0;
}