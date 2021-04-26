#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>


typedef double (*function_t)(double);

typedef struct Arg {
    function_t func;
    double value;
    double *result_ptr;
    sem_t *sem;
} arg_t;

void *thread_safe_func_wrapper(void* arg_ptr) {
    arg_t* arg = (arg_t*)(arg_ptr);
    double value = arg->func(arg->value);
    *(arg->result_ptr) = value;
    sem_post(arg->sem);

    return NULL;
}

double *pmap_process(function_t func, const double *in, size_t count) {
    uint32_t threads_count = get_nprocs();
    pthread_t threads[count];
    sem_t free_processes;
    sem_init(&free_processes, 0, threads_count);

    double *out = malloc(count * sizeof(double));
    arg_t args[count];
    for (int i = 0; i < count; ++i) {
        args[i].sem = &free_processes;
        args[i].value = in[i];
        args[i].result_ptr = &out[i];
        args[i].func = func;
        sem_wait(&free_processes);

        pthread_create(&threads[i], NULL, thread_safe_func_wrapper, &args[i]);
    }
    for (int i = 0; i < threads_count; ++i) {
        sem_wait(&free_processes);
    }
    sem_destroy(&free_processes);

    for (int i = 0; i < count; ++i) {
        pthread_join(threads[i], NULL);
    }
    return out;
}

void pmap_free(double *ptr, size_t count) {
    free(ptr);
}
