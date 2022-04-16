/**
 * @file list.c
 * @author Zijing Zhang (pluvet@foxmail.com)
 * @brief Linux homework, a tool to list files in a directory.
 * @version 0.1
 * @date 2022-04-16
 *
 * @copyright Copyright (c) 2022
 *
 * Examples:
 *
 * ./list -l 100 -h 5000 /bin /etc  # list files in /bin and /etc
 *                                    with size range [100, 5000]
 * ./list -a -r -l 5000 -m 2        # list files recursively in
 *                                    current directory that is
 *                                    larger than 50000 bytes and
 *                                    modified in the last 2 days
 * ./list -- -l                     # just like ./list, ignore args after --
 * ./list                           # list all files in current directory
 *
 */

#pragma once
#define _GNU_SOURCE
#include "dotenv.h"
#include "log.h"
#include "util.h"
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct options
{
  bool recursive;
  bool all;
  size_t min_size;
  size_t max_size;
  unsigned int mod_days;
  int n_dirs;
  char *const *dirs;
};

extern char *optarg;

struct fileinfo
{
  char *path;
  size_t size;
  time_t mod_time;
};

void init_log_level ();
const struct options parse_args (int argc, char *const *argv);
bool is_hidden (const char *bname);
void print_fileinfo (const struct fileinfo *info, struct options const *opts);
void list_files (const char *path, const struct options *opts);
