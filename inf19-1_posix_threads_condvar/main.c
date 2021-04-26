#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <inttypes.h>

uint8_t is_prime(uint64_t num) {
    if (num <= 1) {
        return 0;
    }
    int div = 2;
    while (div*div <= num) {
        if (num % div == 0) {
            return 0;
        }
        ++div;
    }
    return 1;
}

struct argument{
    pthread_cond_t is_ready;
    pthread_mutex_t mutex;
    uint8_t done;
    int32_t N;
    uint64_t A;
    uint64_t B;
    uint64_t num;
};

void *routine(void *argument_ptr) {
    struct argument *arg = (struct argument *) argument_ptr;
    int found_numbers_count = 0;
    pthread_mutex_lock(&arg->mutex);
    for (uint64_t index = arg->A; index <= arg->B; ++index) {
        if (is_prime(index)) {
            arg->num = index;
            arg->done = 1;

            pthread_cond_signal(&arg->is_ready);
            while (arg->done) {
                pthread_cond_wait(&arg->is_ready, &arg->mutex);
            }
            ++found_numbers_count;
            if (arg->N == found_numbers_count) {
                break;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int32_t N = (int) strtol(argv[3], NULL, 10);
    uint64_t A = (int) strtol(argv[1], NULL, 10);
    uint64_t B = (int) strtol(argv[2], NULL, 10);

    struct argument argument = {
            .N=N,
            .A=A,
            .B=B,
            .done=0,
    };
    pthread_mutex_init(&argument.mutex, NULL);
    pthread_cond_init(&argument.is_ready, NULL);

    pthread_t generator;
    pthread_create(&generator, NULL, routine, &argument);

    pthread_mutex_lock(&argument.mutex);
    for (int i = 0; i < N; ++i) {
        while (!argument.done) {
            pthread_cond_wait(&argument.is_ready, &argument.mutex);
        }
        printf("%lu\n", argument.num);

        argument.done = 0;
        pthread_cond_signal(&argument.is_ready);
    }
    pthread_mutex_unlock(&argument.mutex);

    pthread_join(generator, NULL);

    pthread_mutex_destroy(&argument.mutex);


    return 0;
}
