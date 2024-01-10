// Write a simple version of the UNIX find program for xv6: find all the files in a directory tree with a specific name. Your solution should be in the file user/find.c.
// Some hints:

// Look at user/ls.c to see how to read directories.
// Use recursion to allow find to descend into sub-directories.
// Don't recurse into "." and "..".
// Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
// You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
// Note that == does not compare strings like in Python. Use strcmp() instead.
// Add the program to UPROGS in Makefile.
// Your solution is correct if produces the following output (when the file system contains the files b, a/b and a/aa/b):

//     $ make qemu
//     ...
//     init: starting sh
//     $ echo > b
//     $ mkdir a
//     $ echo > a/b
//     $ mkdir a/aa
//     $ echo > a/aa/b
//     $ find . b
//     ./b
//     ./a/b
//     ./a/aa/b
//     $

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
  return buf;
}

void
find(char* path, char* target)
{
    int fd;
    char buf[512], *p;
    struct stat st;
    struct dirent de;
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            if(strcmp(fmtname(path), target) == 0){
                printf("%s %d %d %l\n", path, st.type, st.ino, st.size);
            }
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                    // printf("buf is %s, de.name is %s\n", buf, de.name);
                    find(buf, target);
                }
            }
            break;
    }
    close(fd);
}

int
main(int argc, char* argv[])
{
    if(argc != 3) {
        fprintf(2, "Usage find aa bb\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}