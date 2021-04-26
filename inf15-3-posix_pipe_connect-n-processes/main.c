#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int exec_cmd(char *program_name, int in_fd, int out_fd) {
    dup2(in_fd, 0);
    dup2(out_fd, 1);
    return execlp(program_name, program_name, NULL);
}

void kill_children(int count, int *pid_ids) {
    for (int i = 0; i < count; ++i) {
        kill(pid_ids[i], SIGKILL);
    }
    for (int i = 0; i < count; ++i) {
        waitpid(pid_ids[i], NULL, 0);
    }
}

int main(int argc, char **argv) {
    close(1);
    close(0);
    int a;
    scanf("%d", &a);
    printf("Hello world\n");
    return 0;
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }

    int process_in[argc];
    int process_out[argc];

    for (int i = 1; i < argc - 1; ++i) {
        int pipes[2];
        pipe(pipes);
        process_out[i] = pipes[1];
        process_in[i + 1] = pipes[0];
    }
    process_in[1] = 0;
    process_out[argc - 1] = 1;

    int children[argc];

    for (int i = 1; i < argc; ++i) {
        char *left_program = argv[i];
        pid_t pid = fork();
        if (-1 == pid) {
            kill_children(i - 1, children + 1);
            perror("Fork failed");
            return 1;
        }
        if (pid == 0) {
            for (int j = 1; j < argc; ++j) {
                if (i != j) {
                    close(process_in[j]);
                    close(process_out[j]);
                }
            }
            if (-1 == exec_cmd(left_program, process_in[i], process_out[i])) {
                perror("Exec failed");
                kill_children(i - 1, children + 1);
            }
        }
        children[i] = pid;
    }
    for (int j = 1; j < argc; ++j) {
        close(process_in[j]);
        close(process_out[j]);
    }
    for (int i = 1; i < argc; ++i) {
        waitpid(children[i], NULL, 0);
    }

    return 0;
}