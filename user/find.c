#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

int match(char *re, char *text);
int matchhere(char *re, char *text);
int matchstar(int c, char *re, char *text);

int
match(char *re, char *text)
{
  if(re[0] == '^')
     return matchhere(re + 1, text);
  do {
     if(matchhere(re, text))
       return 1;
  } while (*text++ != '\0');
  return 0;
}

// Helper function to get the basename of a path
char *basename(char *path) {
  char *p = path + strlen(path);
  while(p > path && p[-1] != '/') {
    p--;
  }
  return p;
}

void find (char *path, char *name, char *execargv[], int execargc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0) {
     fprintf(2, "find: cannot open %s\n", path);
     return;
  }
  
  if(fstat(fd, &st) < 0){
     fprintf(2, "find: cannot stat %s\n", path);
     close(fd);
     return;
  }
  
  if(st.type == T_FILE) {
     // Check if the basename of the file matches the pattern
     char *file_basename = basename(path);
     if(match(name, file_basename)){
       if(execargc > 0){
         if(fork() == 0) {
           char *nargv[MAXARG];
           for (int i=0; i< execargc; i++) {
             nargv[i] = execargv[i];
           }
           nargv[execargc] = path;
           nargv[execargc+1] = 0;
           exec(nargv[0], nargv);
           fprintf(2, "exec %s failed\n", nargv[0]);
           exit(1);
         }
         wait(0);
       } else {
         printf("%s\n", path);
       }
     }
     close(fd);
     return;
  }
  
  // if directory exists, iterate entries
  if(st.type == T_DIR) {
     if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
       fprintf(2, "find: path too long\n");
       close(fd);
       return;
     }
     //prepare buffer for child paths
     strcpy(buf, path);
     p = buf + strlen(buf);
     *p++ = '/';
     while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) continue;
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){
          continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        // recursive
        find(buf, name, execargv, execargc);
     }
  }
  close(fd);
}

int
main(int argc, char *argv[]){
  // Support both formats:
  // find startdir name
  // find startdir name -exec cmd [args...]
  
  if(argc < 3) {
     fprintf(2, "usage: find startdir name [-exec cmd [args...]]\n");
     exit(1);
  }
  
  char *execargv[MAXARG];
  int execargc = 0;
  
  // Check if -exec is provided
  if(argc >= 4 && strcmp(argv[3], "-exec") == 0) {
    // -exec format
    if(argc < 5) {
      fprintf(2, "usage: find startdir name -exec cmd [args...]\n");
      exit(1);
    }
    for (int i = 4; i < argc; i++){
      execargv[execargc++] = argv[i];
    }
    execargv[execargc] = 0;
  }
  // If no -exec, execargc remains 0 and find will just print matches
  
  find(argv[1], argv[2], execargv, execargc);
  exit(0);
}

int
matchhere(char *re, char *text)
{
  if(re[0] == '\0')
     return 1;
  if(re[1] == '*')
     return matchstar(re[0], re + 2, text);
  if(re[0] == '$' && re[1] == '\0')
     return *text == '\0';
  if(*text != '\0' && (re[0] == '.' || re[0] == *text))
     return matchhere(re + 1, text + 1);
  return 0;
}

int
matchstar(int c, char *re, char *text)
{
  do {
       if(matchhere(re, text))
          return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}
