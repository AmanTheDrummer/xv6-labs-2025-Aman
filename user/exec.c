#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
     fprintf(2, "Usage: exec command [args...]\n");
     exit(1);
  }
  exec(argv[1], &argv[1]);
  fprintf(2, "exec: failed to run %s\n", argv[1]);
  exit(1);
}
