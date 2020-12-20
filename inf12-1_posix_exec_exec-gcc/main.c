#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define buffer_size 1000000

void wait_child(int pid)
{
    int child_status = 0;
    waitpid(pid, &child_status, 0);
    if (WEXITSTATUS(child_status) != 0) {
        _exit(WEXITSTATUS(child_status));
    }
}

void read_buffer(char* buffer)
{
    int read_bytes = read(STDIN_FILENO, buffer, buffer_size);
    if (-1 == read_bytes) {
        _exit(1);
    }
    buffer[read_bytes] = '\0';
}

int main()
{
    char buffer[buffer_size];
    char temp_filename[] = "print_expr.c";
    char temp_executable[] = "print_expr.out";
    FILE* temp = fopen(temp_filename, "w");
    read_buffer(buffer);
    fprintf(
        temp,
        "#include <stdio.h>\nint main(){printf(\"%%d\\n\",(%s));}",
        buffer);
    fclose(temp);

    int fork_status = fork();
    if (-1 == fork_status) {
        perror("fork");
        return 1;
    }
    if (fork_status == 0) {
        if (-1 ==
            execlp("gcc", "gcc", temp_filename, "-o", temp_executable, NULL)) {
            perror("execlp");
            return 1;
        }
    } else {
        wait_child(fork_status);
        char executable_name[PATH_MAX] = "\0";
        strcat(executable_name, "./");
        strcat(executable_name, temp_executable);
        if (-1 == execlp(executable_name, executable_name, NULL)) {
            perror("execlp");
            return 1;
        }
    }

    return 0;
}