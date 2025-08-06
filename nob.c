#include <stdint.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define SRCDIR "./src/"
#define ODIR SRCDIR "obj/"
#define BINDIR "./bin/"
#define IDIR "./include"
#define TRGT BINDIR "main"
#define SRC SRCDIR "main.c"

int main(int argc, char **argv)
{
  NOB_GO_REBUILD_URSELF(argc, argv);

  shift(argv, argc);

  if (!mkdir_if_not_exists(BINDIR)) return 1;
  if (!mkdir_if_not_exists(ODIR)) return 1;

  Cmd cmd = {0};

  nob_cc(&cmd);
  nob_cmd_append(&cmd, "-c");
  nob_cc_output(&cmd, ODIR "tile.o");
  nob_cc_inputs(&cmd, SRCDIR "tile.c");
  nob_cmd_append(&cmd, "-I" IDIR, "-pg");
  nob_cc_flags(&cmd);

  if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

  nob_cc(&cmd);
  nob_cc_output(&cmd, TRGT);
  nob_cc_inputs(&cmd, SRC, ODIR "tile.o");
  nob_cc_flags(&cmd);
  nob_cmd_append(&cmd, "-ggdb", "-pg");
  // nob_cmd_append(&cmd, "-O3");
  nob_cmd_append(&cmd, "-I" IDIR, "-lraylib");
#ifndef _WIN32
  nob_cmd_append(&cmd, "-lm");
#endif

  if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

  return 0;
}

