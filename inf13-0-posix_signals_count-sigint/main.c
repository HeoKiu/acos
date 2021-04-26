#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sysexits.h>
#include <stdlib.h>

volatile sig_atomic_t sigint_count = 0;
volatile sig_atomic_t should_exit = 0;

void handle_signal(int sig) {
    if (sig == SIGTERM) {
        should_exit = 1;
    } else if (sig == SIGINT) {
        ++sigint_count;
    }
}

int main() {

    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = handle_signal;
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    printf("%d\n", getpid());
    fflush(stdout);
    while(!should_exit){
        pause();
    }
    printf("%d\n", sigint_count);
    fflush(stdout);
    return 0;
}
