#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

void exec_cmd(char *program_name, int in_fd, int out_fd) {
    dup2(in_fd, 0);
    dup2(out_fd, 1);
    execlp(program_name, program_name, NULL);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments\n");
    }
    char *program1 = argv[1];
    char *program2 = argv[2];

    int pipe_fds[2];
    if (-1 == pipe(pipe_fds)) {
        perror("Pipe failed");
    }

    int pipe_read = pipe_fds[0];
    int pipe_write = pipe_fds[1];

    pid_t pid1 = fork();

    if (pid1 != 0) {
        pid_t pid2 = fork();

        if (pid2 != 0) {
            // Parent branch - main process. Waits 2 children
            close(pipe_read);
            close(pipe_write);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        } else {
            // First program - writer
            close(pipe_read);
            exec_cmd(program1, 0, pipe_write);
        }
    } else {
        // Second program - reader
        close(pipe_write);
        exec_cmd(program2, pipe_read, 1);
    }

    return 0;
}