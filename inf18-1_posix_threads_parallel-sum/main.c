#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>


void *sum_numbers(void *sum_ptr) {
    int value;
    while (scanf("%d", &value) == 1) {
        int* sum_cell = sum_ptr;
        *sum_cell += value;
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }
    int threads_count = (int) strtol(argv[1], NULL, 10);

    pthread_t threads[threads_count];
    pthread_t part_sums[threads_count];
    memset(part_sums, 0, sizeof(part_sums));
    int sum = 0;

    for (int i = 0; i < threads_count; ++i) {
        pthread_create(&threads[i], NULL, sum_numbers, part_sums + i);
    }
    for (int i = 0; i < threads_count; ++i) {
        pthread_join(threads[i], NULL);
        sum += part_sums[i];
    }

    printf("%d\n", sum);

    return 0;
}
