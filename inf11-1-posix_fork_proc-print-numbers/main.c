#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    int value = strtol(argv[1], NULL, 10);
    int init_value = value;

    while (value >= 1) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            return 1;
        } else if (0 == child_pid) {
            --value;
        } else {
            int status = 0;
            waitpid(child_pid, &status, 0);
            if(value == init_value){
                printf("%d\n", value);
            }else{
                printf("%d ", value);
            }
            return 0;
        }
    }
    return 0;
}
