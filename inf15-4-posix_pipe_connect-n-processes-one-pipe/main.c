#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>


int launch_program(char *program, int in_fd, const int *right_pipe) {
    int out_fd = right_pipe[1];
    pid_t child_fd = fork();
    if (-1 == child_fd) {
        return -1;
    }
    if (0 == child_fd) {
        close(right_pipe[0]);
        dup2(in_fd, 0);
        dup2(out_fd, 1);
        execlp(program, program, NULL);
        exit(1);
    }
    close(in_fd);
    waitpid(child_fd, NULL, 0);
    close(out_fd);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Too few arguments\n");
    }

    int read_fd = 0;
    int right_pipe[2];
    pipe(right_pipe);

    for (int i = 1; i < argc; ++i) {
        if (i == argc - 1) {
            close(right_pipe[1]);
            right_pipe[1] = 1;
        }
        if (-1 == launch_program(argv[i], read_fd, right_pipe)) {
            break;
        }

        read_fd = right_pipe[0];
        pipe(right_pipe);

    }

    return 0;
}
