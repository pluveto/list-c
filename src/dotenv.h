#pragma once
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int set_env (const char *key, const char *value);
int load_env (const char *path);
int load_dotenv ();
const char *get_env (const char *key, const char *def);
