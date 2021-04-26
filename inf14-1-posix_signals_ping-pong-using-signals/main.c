#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void handle_signal(int sig, siginfo_t *info, void *ptr) {
    int value = info->si_int;
    int sender_pid = info->si_pid;
    if (value == 0){
        exit(0);
    }
    sigqueue(sender_pid, sig, (sigval_t) (value - 1));
}

int main(int argc, char **argv) {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGRTMIN);

    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_sigaction = handle_signal;
    signal_action.sa_flags = SA_SIGINFO;
    signal_action.sa_mask = mask;
    sigaction(SIGRTMIN, &signal_action, NULL);

//    printf("%d\n", getpid());
//    fflush(stdout);

    while (1) {
        pause();
    }
    return 0;
}