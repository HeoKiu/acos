#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdatomic.h>

typedef struct Item {
    struct Item *next;
    int64_t value;
} item_t;

typedef struct List {
    item_t *head;
    item_t *tail;
    size_t size;
} list_t;

void list_push(list_t *list, int64_t value) {
    item_t *item = malloc(sizeof(item_t));
    item->value = value;
    item->next = NULL;
    item_t *tail = atomic_exchange(&list->tail, item);
    if(tail == NULL){
        list->head = item;
    }else{
        tail->next = item;
    }
    atomic_fetch_add(&list->size, 1);
}

void list_init(list_t **list) {
    *list = malloc(sizeof(list_t));
    (*list)->size = 0;
    (*list)->head = NULL;
    (*list)->tail = NULL;
}

void list_destroy(list_t *list) {
    item_t *item = list->head;
    while (item != NULL) {
        item_t *curr_item = item;
        item = item->next;
        free(curr_item);
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}


list_t *list = NULL;
int k = 0;

void *worker(void *arg_ptr) {
    uint64_t index = (uint64_t) arg_ptr;

    for (int64_t i = index * k; i < (index + 1) * k; ++i) {
        list_push(list, i);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments");
        return 1;
    }

    int n = (int) strtol(argv[1], NULL, 10);
    k = (int) strtol(argv[2], NULL, 10);

    list_init(&list);

    pthread_t threads[n];

    for (uint64_t i = 0; i < n; ++i) {
        pthread_create(&threads[i], NULL, worker, (void *) i);
    }

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }

    item_t *item = list->head;
    for (int i = 0; i < list->size; ++i) {
        printf("%ld\n", item->value);
        item = item->next;
    }

    list_destroy(list);

    return 0;
}
