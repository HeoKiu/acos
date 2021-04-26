#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t value = 0;
volatile sig_atomic_t should_exit = 0;

void handle_signal(int sig) {
    switch (sig) {
        case SIGUSR1:
            ++value;
            printf("%d\n", value);
            fflush(stdout);
            break;
        case SIGUSR2:
            value *= -1;
            printf("%d\n", value);
            fflush(stdout);
            break;
        default:
            should_exit = 1;
    }
}

int main() {

    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = handle_signal;
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    sigaction(SIGUSR1, &signal_action, NULL);
    sigaction(SIGUSR2, &signal_action, NULL);

    printf("%d\n", getpid());
    fflush(stdout);
    scanf("%d", &value);
    while(!should_exit){
        pause();
    }
    return 0;
}