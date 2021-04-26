#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sem_t* smoked_;
sem_t* resources_;
pid_t pid_indx_[3];

void to_hand() {
    char to_hand;
    while (scanf("%c", &to_hand) > 0){
        size_t id = 2;
        if (to_hand == 'p') {
            id = 1;
        }
        if (to_hand == 't') {
            id = 0;
        }
        sem_post(&resources_[id]);
        sem_wait(&smoked_[id]);
    }

    for (size_t smoker_indx = 0; smoker_indx < 3; ++smoker_indx) {
        kill(pid_indx_[smoker_indx], SIGTERM);
        waitpid(pid_indx_[smoker_indx], 0, 0);
    }

    // free resources
    munmap(smoked_, sizeof(sem_t) * 3);
    munmap(resources_, sizeof(sem_t) * 3);
}

void handler(int signum) {
    _exit(0);
}

void to_smoke(size_t smoker_id) {
    // signal action
    struct sigaction sigact;
    sigact.sa_flags = SA_RESTART;
    sigact.sa_handler = handler;
    sigaction(SIGTERM, &sigact, NULL);

    char was_given;

    if (smoker_id == 1) {
        was_given = 'P';
    }
    else if (smoker_id == 0) {
        was_given = 'T';
    }
    else {
        was_given = 'M';
    }

    for (;;) {
        sem_wait(&resources_[smoker_id]);
        printf("%c\n", was_given);
        fflush(stdout);
        sem_post(&smoked_[smoker_id]);
    }
}

int main(int argc, char* argv[]) {
    int semaphores_size = 3 * sizeof(sem_t);
    size_t curr_indx = 3;
    smoked_ = mmap(NULL, semaphores_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    resources_ = mmap(NULL, semaphores_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    for (size_t sem_indx = 0; sem_indx < 3; sem_indx++) {
        sem_init(&smoked_[sem_indx], 1, 0);
        sem_init(&resources_[sem_indx], 1, 0);
    }

    for (size_t pid_indx = 0; pid_indx < 3; pid_indx++) {
        pid_t pid = fork();
        if (pid != 0) {
            pid_indx_[pid_indx] = pid;
        }
        else {
            curr_indx = pid_indx;
            break;
        }
    }

    if (3 != curr_indx) {
        to_smoke(curr_indx);
    }
    else {
        to_hand();
    }
}