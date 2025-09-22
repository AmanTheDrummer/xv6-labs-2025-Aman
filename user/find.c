#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find (char *path, char *name) 
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0) {
     fprintf(2, "find: cannor open %s\n", path);
     return;
  }
  if(fstat(fd, &st) < 0){
     fprintf(2, "find: cannot stat %s\n", path);
     close(fd);
     return;
  }

  if(st.type == T_FILE) {
     // isolate last component of path
     char *last = path;
     for(char *c = path; *c; c++)
       if(*c == '/') last = c + 1;
     if(strcmp(last, name) == 0)
       printf("%s\n", path);
     close(fd);
     return;
  }
  
  // if directory exists, iterate entries
  if(st.type == T_DIR) {
     //prepare buffer for child paths
     strcpy(buf, path);
     p = buf + strlen(buf);
     *p++ = '/';
     while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        // recursive
        find(buf, name);
     }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3) {
     fprintf(2, "usage: find <startdir> <filename>\n");
     exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
