#include <semaphore.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>
#include <stdbool.h>


volatile sig_atomic_t kick_out = 0;

void signal_handler(int signal) {
    kick_out = true;
}

void set_sigterm_handler() {
    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGTERM, &signal_action, NULL);
}


void run_smoker(char type, sem_t *sem_ptr, sem_t *done_sem_ptr, pid_t *smoker_pid) {
    pid_t pid = fork();
    if (pid != 0) {
        *smoker_pid = pid;
        return;
    }
    set_sigterm_handler();

    while (!kick_out) {
        sem_wait(sem_ptr);
        if(kick_out){
            break;
        }
        printf("%c\n", type);
        fflush(stdout);
        sem_post(done_sem_ptr);
    }
    exit(0);
}

const int n_smokers = 3;

int main() {

    sem_t *semaphores = mmap(NULL, sizeof(sem_t) * n_smokers, PROT_WRITE | PROT_READ,
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_t *done_action = mmap(NULL, sizeof(sem_t) * n_smokers, PROT_WRITE | PROT_READ,
                              MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    char *types = "TPM";
    pid_t smokers[n_smokers];

    for (int i = 0; i < n_smokers; ++i) {
        sem_init(&semaphores[i], 1, 0);
        sem_init(&done_action[i], 1, 0);
        run_smoker(types[i], &semaphores[i], &done_action[i], &smokers[i]);
    }

    char action;
    while (scanf("%c", &action) > 0) {
        if (action == 't') {
            sem_post(&semaphores[0]);
            sem_wait(&done_action[0]);
        }
        if (action == 'p') {
            sem_post(&semaphores[1]);
            sem_wait(&done_action[1]);
        }
        if (action == 'm') {
            sem_post(&semaphores[2]);
            sem_wait(&done_action[2]);
        }
    }
    for (int i = 0; i < n_smokers; ++i) {
        kill(smokers[i], SIGTERM);
    }

    for (int i = 0; i < n_smokers; ++i) {
        waitpid(smokers[i], NULL, 0);
    }

    return 0;
}