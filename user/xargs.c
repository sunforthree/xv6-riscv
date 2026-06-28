#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

void
execute_line(char *line, char *args[], int arg_count) {
    int ws = 1; // Flag to indicate if we are in whitespace
    char **arg, *l;

    // Split input line into words based on whitespace
    arg = &args[arg_count]; // Start filling args after the initial arguments
    l = line;

    while (*l != '\0') {
        if (*l == ' ') {
            if (!ws) {
                *l = '\0';
                arg++;
            }
            ws = 1;
        } else {
            if (ws) {
                *arg = l;
            }
            ws = 0;
        }
        l++;
    }

    int f = fork();
    if (f < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    else if (f == 0) {
        // Child process: execute the command
        exec(args[0], args);
        fprintf(2, "exec %s failed\n", args[0]);
        exit(1);
    } else {
        // Parent process: wait for the child to finish
        wait(0);
    }
}

int
main(int argc, char *argv[])
{
    char *args[MAXARG] = { 0 };
    char buf[512], *b;
    b = buf; // Pointer to the buffer

    // Copy the command-line arguments into the buffer
    for (int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
        // printf("args[%d]: %s\n", i - 1, args[i - 1]); // Debugging output
    }

    // Read from standard input
    while (read(0, b, 1) > 0) {
        if (*b == '\n') {
            *b = '\0'; // Null-terminate the string
            // printf("Executing command: %s\n", buf); // Debugging output
            execute_line(buf, args, argc - 1); // Execute the command with arguments
            b = buf; // Reset the buffer pointer for the next command
        } else {
            b++; // Move to the next character in the buffer
        }
    }

    // Execute any remaining command in the buffer if it doesn't end with a newline
    if (b != buf) {
        *b = '\0';
        execute_line(buf, args, argc - 1);
    }

    exit(0);
}