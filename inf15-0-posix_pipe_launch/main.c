#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments\n");
    }
    char *program_name = argv[1];
    char *file_name = argv[2];

    int pipe_fds[2];
    if (-1 == pipe(pipe_fds)){
        perror("Pipe failed");
    }

    int pipe_read = pipe_fds[0];
    int pipe_write = pipe_fds[1];

    pid_t pid = fork();

    if (pid == 0) {
        close(pipe_read);
        int file_fd = open(file_name, O_RDONLY);
        dup2(file_fd, 0);
        dup2(pipe_write, 1);
        execlp(program_name, program_name, NULL);
    } else {
        close(pipe_write);
        int bytes_in_pipe = 0;
        int read_bytes = 0;
        int buffer_size = 100;
        char buffer[buffer_size];
        while(0 < (read_bytes = read(pipe_read, buffer, buffer_size))){
            bytes_in_pipe += read_bytes;
        }
        printf("%d\n", bytes_in_pipe);
        close(pipe_read);
        wait(NULL);
    }

    return 0;
}
