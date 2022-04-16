## list-c

This is a list tool like `ls` with less features. It lists files (no dir). It's one of my Linux course homework from BUPT.

## Usage

```
usage: ./build/list [-arl:m:h:] [path...]
  -a  list all files
  -r  recursive
  -l  min size
  -h  max size
  -m  max days
args after -- will be ignored
```

Examples:

```
./list -l 100 -h 5000 /bin /etc  # list files in /bin and /etc
                                   with size range [100, 5000]
./list -a -r -l 5000 -m 2        # list files recursively in
                                   current directory that is
                                   larger than 50000 bytes and
                                   modified in the last 2 days
./list -- -l                     # just like ./list, ignore args after --
./list                           # list all files in current directory
```

## Build

```shell
make build
```

## Debugging

```shell
make debug
# or:
make gdb_debug
```

You can specific a log level in `.env` file or environment variable:

```
LOG_LEVEL=<one of TRACE
                  DEBUG
                  INFO
                  WARN
                  ERROR
                  FATAL>
```