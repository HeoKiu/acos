#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_id = 0;
    int status;
    int max_processes_count = 1;
    while (1) {
        child_id = fork();
        if (-1 == child_id) {
            printf("%d", max_processes_count);
            return 0;
        }else if(child_id != 0) {
            waitpid(child_id, &status, 0);
            return 0;
        }
    }
}
