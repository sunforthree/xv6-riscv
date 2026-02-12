#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MIN 2
#define MAX 280

int
process(int in_pipe, int prime)
{
    int num;
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0)
    {
        close(p[0]);
        while (read(in_pipe, &num, sizeof(int)) == 4) {
            if (num % prime != 0) {
                write(p[1], &num, sizeof(int));
            }
        }
        close(in_pipe);
        close(p[1]);
        exit(0);
    }

    close(in_pipe);
    close(p[1]);
    return p[0];
}

int
generate()
{
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0)
    {
        for (int i = MIN; i < MAX; ++i) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        exit(0);
    }

    close(p[1]);
    return p[0];
}

int
main(int argc, char *argv[])
{

    int num;

    int in_pipe = generate();

    while (read(in_pipe, &num, sizeof(int)) == 4) {
        printf("prime %d\n", num);
        in_pipe = process(in_pipe, num);
    }

    wait(0);


    exit(0);
}