#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  if(argc < 2) {
    fprintf(2, "Usage: sixfive file ...\n");
    exit(1);
  }
  // loop over every given filename
  for (int f = 1; f < argc; f++) {
    int fd = open(argv[f], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[f]);
      continue;
    }
 
    char buf[512];
    int n;
    int num = 0;
    int in_num = 0;

    // read file in chunks
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
      for (int i = 0; i < n; i++) {
        char c = buf[i];
        if (c >= '0' && c <= '9') {
          num = num * 10 + (c - '0');
          in_num = 1;
        } else {
          if (in_num) {
            if (num % 6 == 0 || num % 5 == 0)
              printf("%d\n", num);
            num = 0;
            in_num = 0;
          }
        }
      }
    }
    if (in_num) {
      if(num % 6 == 0 || num % 5 == 0) {
        printf("%d\n", num);
      }
    } 
    close(fd);
  }
  exit(0);

}
