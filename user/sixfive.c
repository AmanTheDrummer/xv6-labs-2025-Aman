#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int max = 100;
  if(argc > 1) max = atoi(argv[1]);

  for (int i = 1; i <= max; i++){
     if(i % 6 == 0 || i % 5 == 0)
       printf("%d\n", i);
  }
  exit(0);
}
