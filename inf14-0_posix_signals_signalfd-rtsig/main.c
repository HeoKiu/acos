#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES_COUNT 100

volatile sig_atomic_t value = 0;
volatile sig_atomic_t should_exit = 0;
FILE *files_list[MAX_FILES_COUNT];
char *line = NULL;

void handle_signal(int sig) {
    if (sig == SIGRTMIN) {
        should_exit = 1;
        return;
    }
    int file_no = sig - SIGRTMIN;
    size_t read_line_len;
    FILE *file = files_list[file_no];
    getline(&line, &read_line_len, file);
    printf("%s", line);
    if(line){
        free(line);
        line = NULL;
    }
    fflush(stdout);
}

int main(int argc, char **argv) {

    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = handle_signal;
    sigaction(SIGINT, &signal_action, NULL);
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        sigaction(i, &signal_action, NULL);
    }
    for (int i = 1; i < argc; ++i) {
        files_list[i] = fopen(argv[i], "r");
    }

//    printf("%d\n", getpid());
//    fflush(stdout);

    while (!should_exit) {
        pause();
    }
    for (int i = 1; i < argc; ++i) {
        fclose(files_list[i]);
    }
    return 0;
}