#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <sched.h>

typedef struct Item {
    struct Item* next;
    int value;
} item_t;

void print_list(item_t* cur) {
    printf("%d\n", cur->value);
    if (cur->next != NULL) {
        print_list(cur->next);
    }
    return;
}

void add_item(_Atomic(item_t*)* list, int i) {
    item_t* new_item = malloc(sizeof(item_t));
    new_item->next = atomic_exchange(list, new_item);
    new_item->value = i;
}

typedef struct {
    _Atomic(item_t*)* list;
    int start_val;
    int end_val;
    _Atomic int* in_work;
} routine_args_t;

static void* fill_list(void* arg) {
    routine_args_t args = *(routine_args_t*)arg;
    for (int i = args.start_val; i < args.end_val; ++i) {
        add_item(args.list, i);
        sched_yield();
    }
    return NULL;
}

int main(int argc, char** argv) {
    int N = atoi(argv[1]);
    int k = atoi(argv[2]);

    _Atomic(item_t*) head = NULL;
    _Atomic int my_atomic = 0;

    routine_args_t args[N];
    for (int i = 0; i < N; ++i) {
        args[i].list = &head;
        args[i].start_val = i * k;
        args[i].end_val = (i + 1) * k;
        args[i].in_work = &my_atomic;
    }

    pthread_t worker[N];
    for (int i = 0; i < N; ++i) {
        pthread_create(&worker[i], NULL, fill_list, (void*)&args[i]);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(worker[i], NULL);
    }
    // printf("OK");
    // return 0;
    // printf("%d", (head == NULL));
    // return 0;
    print_list(head);
    return 0;
}