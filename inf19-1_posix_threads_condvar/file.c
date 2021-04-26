#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
} routine_guard_t;

typedef struct {
    routine_guard_t* guard;
    int64_t A;
    int64_t B;
    int32_t N;
    int64_t* prime_number;
} routine_arg_t;

bool prime(int64_t number) {
    if (number <= 1) {
        return false;
    }
    if (number % 2 == 0) {
        return number == 2;
    }
    for (int64_t i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

static void* find_primes(void *arg) {
    routine_arg_t args = *(routine_arg_t*)arg;
    int32_t finded = 0;

    pthread_mutex_lock(&args.guard->mutex); // start of critical section
    for (int64_t i = args.A; i <= args.B; ++i) {
        if (prime(i)) {
            *args.prime_number = i;
            pthread_mutex_unlock(&args.guard->mutex); // end of critical section

            pthread_cond_signal(&args.guard->condvar); // notifing main function

            pthread_mutex_lock(&args.guard->mutex); // start of critical section
            while (*args.prime_number != -1) {
                pthread_cond_wait(&args.guard->condvar, &args.guard->mutex);
            }
            finded++;
            if (finded == args.N) {
                break;
            }
        }
    }
    *args.prime_number = -2;
    pthread_mutex_unlock(&args.guard->mutex); // end of critical section

    pthread_cond_signal(&args.guard->condvar); // notifing main function
    return NULL;
}

int main(int argc, char** argv) {
    int64_t A, B;
    int32_t N;
    sscanf(argv[1], "%" SCNd64, &A);
    sscanf(argv[2], "%" SCNd64, &B);
    sscanf(argv[3], "%" SCNd32, &N);

    int64_t prime_number = -1;
    routine_guard_t guard = {
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .condvar = PTHREAD_COND_INITIALIZER,
    };
    routine_arg_t args = {
            .guard = &guard,
            .A = A,
            .B = B,
            .N = N,
            .prime_number = &prime_number
    };

    pthread_t worker;
    pthread_create(&worker, NULL, find_primes, (void*)&args);

    {
        int32_t readed = 0;
        pthread_mutex_lock(&guard.mutex);
        while (readed < N) {
            pthread_mutex_unlock(&guard.mutex);

            pthread_cond_signal(&guard.condvar);

            pthread_mutex_lock(&guard.mutex);
            while (prime_number == -1) {
                pthread_cond_wait(&guard.condvar, &guard.mutex);
            }
            fflush(stdout);
            if (prime_number == -2) {
                break;
            } else {
                printf("%" PRId64 "\n", prime_number);
                readed++;
                prime_number = -1;
            }
        }
        pthread_mutex_unlock(&guard.mutex);

        pthread_cond_signal(&guard.condvar);
    }
    pthread_join(worker, NULL);
    return 0;
}