#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }
    char *program_name = argv[1];
    int program_name_len = (int) strlen(program_name);

    int pipes[2];
    pipe(pipes);

    pid_t pid = fork();
    if (-1 == pid) {
        perror("Fork failed");
        return 1;
    }

    int read_fd = pipes[0];
    int write_fd = pipes[1];

    if (pid == 0) {
        close(read_fd);
        dup2(write_fd, 2);
        dup2(write_fd, 1);
        execlp("gcc", "gcc", program_name, NULL);
    } else {
        close(write_fd);
        FILE *gcc_err = fdopen(read_fd, "r");

        int error_lines = 0;
        int warning_lines = 0;

        char *line = NULL;
        size_t len;
        int str_no, last_err_no = -1, last_warn_no = -1;

        while (getline(&line, &len, gcc_err) != -1) {
            if (strncmp(line, program_name, program_name_len) != 0) {
                continue;
            }
            char* line_ptr;
            str_no = (int) strtol(line + program_name_len + 1, &line_ptr, 10);
            if(str_no == 0){
                continue;
            }
            // + 1 skip ':'
            strtol(line_ptr + 1, &line_ptr, 10);
            line_ptr += 2; // 2 symbols ':' and ' '
            if(strncmp(line_ptr, "error", 5) == 0){
                error_lines += str_no != last_err_no;
                last_err_no = str_no;
            }
            if(strncmp(line_ptr, "warning", 7) == 0){
                warning_lines += str_no != last_warn_no;
                last_warn_no = str_no;
            }
        }

        printf("%d %d\n", error_lines, warning_lines);
    }

    return 0;
}
