// Write a user-level program that uses xv6 system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. The parent should send a byte to the child; the child should print "<pid>: received ping", where <pid> is its process ID, write the byte on the pipe to the parent, and exit; the parent should read the byte from the child, print "<pid>: received pong", and exit. Your solution should be in the file user/pingpong.c.

// Some hints:

// Add the program to UPROGS in Makefile.
// Use pipe to create a pipe.
// Use fork to create a child.
// Use read to read from a pipe, and write to write to a pipe.
// Use getpid to find the process ID of the calling process.
// User programs on xv6 have a limited set of library functions available to them. You can see the list in user/user.h; the source (other than for system calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
// Run the program from the xv6 shell and it should produce the following output:

//     $ make qemu
//     ...
//     init: starting sh
//     $ pingpong
//     4: received ping
//     3: received pong
//     $

#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int p1[2];
    int p2[2];

    pipe(p1);
    pipe(p2);

    if(fork() == 0) {
        close(p1[1]);
        close(p2[0]);
        char buf[512];
        read(p1[0], buf, sizeof buf);
        close(p1[0]);
        int cpid = getpid();
        printf("%d: %s\n", cpid, buf);
        const char* msg = "received pong";
        write(p2[1], msg, sizeof msg);
        close(p2[1]);
        exit(0);
    } else {
        close(p1[0]);
        close(p2[1]);
        const char* msg = "received ping";
        write(p1[1], msg, sizeof msg);
        close(p1[1]);
        char buf[512];
        read(p2[0], buf, sizeof buf);
        close(p2[0]);
        int ppid = getpid();
        printf("%d: %s\n", ppid, buf);
        wait(0);
        exit(0);
    }
    exit(0);
}