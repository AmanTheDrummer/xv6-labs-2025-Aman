#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int arg_count, char* arg_vector[]) {
  if(arg_count < 2) {
     fprintf(2, "Usage: sleep ticks\n");
     exit(1);
  }
  int ticks = atoi(arg_vector[1]);
  pause(ticks);
  exit(0);
}
