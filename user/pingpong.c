#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2], p2[2];
    char buf[10];
    pipe(p);
    pipe(p2);

    int pid = fork();    
    if (pid == 0) {
        // Child process
        close(p[1]);
        read(p[0], buf, 5);
        printf("%d: received %s\n", getpid(), buf);
        write(p2[1], "pong", 5);
        close(p2[1]);
    }
    else if (pid > 0){
        // Parent process
        close(p2[1]);
        write(p[1], "ping", 5);
        read(p2[0], buf, 5);
        printf("%d: received %s\n", getpid(), buf);
        close(p[1]);
    }
    else {
        // Fork failed
        fprintf(2, "Fork failed\n");
        exit(1);
    }

    exit(0);
}