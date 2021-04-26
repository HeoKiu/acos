#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>



void handle_signal(int sig) {
    if (sig == SIGCHLD){
        int status = 0;
        wait(&status);
        if(WIFEXITED(status)){
            printf("ok\n");
            exit(0);
        }
        if(WIFSIGNALED(status)){
            printf("signaled\n");
            exit(1);
        }
    }
}

void prohibit_all_signals(){
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

void set_signals_handler(){
    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = handle_signal;
    sigaction(SIGCHLD, &signal_action, NULL);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }
    int seconds = (int) strtol(argv[1], NULL, 10);
    pid_t pid = fork();
    if (-1 == pid) {
        perror("Fork failed");
        return 1;
    }
    if (pid != 0) {
        set_signals_handler();
        sleep(seconds);

        prohibit_all_signals();

        kill(pid, SIGTERM);
        wait(NULL);
        printf("timeout\n");

        return 2;
    } else {
        execvp(argv[2], argv + 2);
        perror("Exec failed");
        return 1;
    }
}
