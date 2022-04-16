#include "list.h"

static const char *level_strings[]
    = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

int
main (int argc, char *const *argv)
{
  load_dotenv ();
  init_log_level ();
  log_debug ("start");
  const struct options opt = parse_args (argc, argv);
  log_debug ("args: recursive: %d, all: %d, min_size: %zu, max_size: %zu, "
             "mod_days: %u",
             opt.recursive, opt.all, opt.min_size, opt.max_size, opt.mod_days);
  log_debug ("optind: %d", optind);
  if (opt.n_dirs == 0)
    {
      log_debug ("no path specified, use current directory");
      list_files (".", &opt);
    }
  else
    {
      for (int i = 0; i < opt.n_dirs; i++)
        {
          log_debug ("list files in %s", opt.dirs[i]);
          list_files (opt.dirs[i], &opt);
        }
    }
  return 0;
}

void
init_log_level ()
{
  int log_level = 0;
  const char *level = get_env ("LOG_LEVEL", "INFO");
  for (int i = 0; i < sizeof (level_strings) / sizeof (level_strings[0]); i++)
    {
      if (strcmp (level, level_strings[i]) == 0)
        {
          log_level = i;
          break;
        }
    }
  log_set_level (log_level);
}

const struct options
parse_args (int argc, char *const *argv)
{
  struct options opts = {
    .recursive = false,
    .all = false,
    .min_size = 0,
    .max_size = SIZE_MAX,
    .mod_days = 0,
    .dirs = NULL,
    .n_dirs = 0,
  };
  int opt;
  while (optind < argc)
    {
      if (strcmp (argv[optind], "--") == 0)
        {
          optind++;
          break;
        }

      if ((opt = getopt (argc, argv, "arl:m:h:")) != -1)
        {
          switch (opt)
            {
            case 'a':
              opts.all = true;
              break;
            case 'r':
              opts.recursive = true;
              break;
            case 'l':
              opts.min_size = atoi (optarg);
              break;
            case 'm':
              opts.mod_days = atoi (optarg);
              break;
            case 'h':
              opts.max_size = atoi (optarg);
              break;
            default:
              if (opt == '?')
                {
                  fprintf (stderr,
                           "usage: %s [-arl:m:h:] [path...]\n"
                           "  -a  list all files\n"
                           "  -r  recursive\n"
                           "  -l  min size\n"
                           "  -h  max size\n"
                           "  -m  max days\n"
                           "args after -- will be ignored\n",
                           argv[0]);

                  exit (EXIT_FAILURE);
                }
              break;
            }
        }
      else
        {
          opts.dirs = &argv[optind];
          opts.n_dirs = argc - optind;
          break;
        }
    }
  return opts;
}

bool
is_hidden (const char *bname)
{
  return bname[0] == '.';
}

void
print_fileinfo (const struct fileinfo *info, struct options const *opts)
{
  char *bname = basename (info->path);
  if (!opts->all && is_hidden (bname))
    {
      log_debug ("ignore hidden file: %s", bname);
      return;
    }
  if (opts->min_size > 0 && info->size < opts->min_size)
    {
      log_debug ("ignore file with size < %d: %s", opts->min_size, bname);
      return;
    }
  if (opts->max_size < SIZE_MAX && info->size > opts->max_size)
    {
      log_debug ("ignore file with size > %d: %s", opts->max_size, bname);
      return;
    }
  if (opts->mod_days > 0)
    {
      time_t now = time (NULL);
      time_t mod_time = info->mod_time;
      if (now - mod_time > opts->mod_days * 24 * 60 * 60)
        {
          log_debug ("ignore file not modified in the last %d day(s): %s",
                     opts->mod_days, bname);
          return;
        }
    }
  // printf ("%s\t%ld\t%s", info->path, info->size, ctime (&info->mod_time));
  printf ("%10ld %s\n", info->size, info->path);
}

void
list_files (const char *path, const struct options *opts)
{
  DIR *dir = opendir (path);
  if (!dir)
    {
      log_error ("opendir %s failed", path);
      fprintf (stderr, "opendir %s failed, is it existing?\n", path);
      return;
    }

  struct dirent *dirent;
  while ((dirent = readdir (dir)) != NULL)
    {
      if (strcmp (dirent->d_name, ".") == 0
          || strcmp (dirent->d_name, "..") == 0)
        {
          continue;
        }

      char full_path[PATH_MAX];
      if (strcmp (path, "/") == 0)
        {
          snprintf (full_path, sizeof (full_path), "/%s", dirent->d_name);
        }
      else
        {
          snprintf (full_path, sizeof (full_path), "%s/%s", path,
                    dirent->d_name);
        }
      struct stat statbuf;
      if (lstat (full_path, &statbuf) == -1)
        {
          log_error ("lstat %s failed", full_path);
          fprintf (stderr, "lstat %s failed\n", full_path);
          continue;
        }

      if (S_ISDIR (statbuf.st_mode))
        {
          if (opts->recursive)
            {
              list_files (full_path, opts);
            }
          continue;
        }

      struct fileinfo info = {
        .path = full_path,
        .size = statbuf.st_size,
        .mod_time = statbuf.st_mtime,
      };
      print_fileinfo (&info, opts);
    }

  closedir (dir);
}
