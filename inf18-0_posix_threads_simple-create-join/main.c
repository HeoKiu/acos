#include <stdio.h>
#include <pthread.h>


void *number_reader(void *arg) {
    int value;
    if(scanf("%d", &value) < 1){
        return NULL;
    }
    pthread_t thread;
    pthread_create(&thread, NULL, number_reader, NULL);
    pthread_join(thread, NULL);
    printf("%d\n", value);
    return NULL;
}

int main() {

    number_reader(NULL);

    return 0;
}
