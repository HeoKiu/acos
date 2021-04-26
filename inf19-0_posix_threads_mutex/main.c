#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct argument {
    pthread_mutex_t *mutex;
    double *array;
    size_t length;
    size_t N;
    size_t idx;
};

void *routine(void *argument_ptr) {
    struct argument *argument = (struct argument *) argument_ptr;
    for (int i = 0; i < argument->N; ++i) {
        pthread_mutex_lock(argument->mutex);

        argument->array[(argument->idx + argument->length - 1) % argument->length] += .99;
        argument->array[argument->idx] += 1;
        argument->array[(argument->idx + argument->length + 1) % argument->length] += 1.01;

        pthread_mutex_unlock(argument->mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int N = (int) strtol(argv[1], NULL, 10);
    int K = (int) strtol(argv[2], NULL, 10);
    pthread_t thread_objects[K];
    struct argument arguments[K];
    double array[K];

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < K; ++i) {
        array[i] = 0;
    }

    for (int i = 0; i < K; ++i) {
        struct argument arg = {
                .array = array,
                .idx = i,
                .N = N,
                .length = K,
                .mutex = &mutex
        };
        arguments[i] = arg;
        pthread_create(&thread_objects[i], NULL, routine, &arguments[i]);
    }
    for (int i = 0; i < K; ++i) {
        pthread_join(thread_objects[i], NULL);
    }
    for (int i = 0; i < K; ++i) {
        printf("%.10g\n", array[i]);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}
