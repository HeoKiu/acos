#include <stdio.h>
#include <zconf.h>
#include <wait.h>

int main() {

    int status = 0;
    char buffer[4096];
    int in_child = 0;

    while (1) {
        if (EOF == scanf("%s", buffer)) {
            if (!in_child) {
                printf("0\n");
            }
            return 0;
        }
        pid_t child_pid = fork();
        if (-1 == child_pid) {
            perror("fork");
            return 0;
        } else if (0 != child_pid) {
            waitpid(child_pid, &status, 0);
            int words_count = WEXITSTATUS(status) + 1;
            if (in_child) {
                return words_count;
            }
            printf("%d\n", words_count);
            return 0;
        }
        in_child = 1;
    }
}
