#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <unistd.h>
#include <inttypes.h>


void *first_thread_func(void *sock_ptr) {
    int sock_fd = *(int *) sock_ptr;
    int value;
    while (1) {
        read(sock_fd, &value, sizeof(uint32_t));
        if (value == 200) {
            break;
        }
        value -= 3;
        printf("%d\n", value);
        fflush(stdout);
        if (value == 0 || value > 100) {
            value = 200;
            write(sock_fd, &value, sizeof(uint32_t));
            break;;
        }
        write(sock_fd, &value, sizeof(uint32_t));
    }
    return NULL;
}

void *second_thread_func(void *sock_ptr) {
    int sock_fd = *(int *) sock_ptr;
    int value;
    while (1) {
        read(sock_fd, &value, sizeof(uint32_t));
        if (value == 200) {
            break;
        }
        value += 5;
        printf("%d\n", value);
        fflush(stdout);
        if (value == 0 || value > 100) {
            value = 200;
            write(sock_fd, &value, sizeof(uint32_t));
            break;;
        }
        write(sock_fd, &value, sizeof(uint32_t));
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }
    uint32_t initial_value = (int) strtol(argv[1], NULL, 10);

    int socket_pair[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);

    pthread_t first_thread;
    pthread_t second_thread;

    write(socket_pair[1], &initial_value, sizeof(uint32_t));

    pthread_create(&first_thread, NULL, first_thread_func, &socket_pair[0]);
    pthread_create(&second_thread, NULL, second_thread_func, &socket_pair[1]);

    pthread_join(first_thread, NULL);
    pthread_join(second_thread, NULL);

    close(socket_pair[0]);
    close(socket_pair[1]);

    return 0;
}
