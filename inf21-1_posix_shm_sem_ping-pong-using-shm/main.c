#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define _GNU_SOURCE
#include <dlfcn.h>


typedef struct {
    sem_t request_ready;  // начальное значение 0
    sem_t response_ready; // начальное значение 0
    char func_name[20];
    double value;
    double result;
} shared_data_t;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return 1;
    }
    char *filename = argv[1];

    double (*function)(double);

    void *lib = dlopen(filename, 0);

    char *shm_name = "/fivol_shm";

    int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0777);
    ftruncate(fd, sizeof(shared_data_t));
    shared_data_t *data = mmap(NULL, sizeof(shared_data_t), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    sem_init(&data->request_ready, 1, 0);
    sem_init(&data->response_ready, 1, 0);
    data->value = 0;
    data->result = 0;
    data->func_name[0] = '\0';

    printf("%s\n", shm_name);
    fflush(stdout);

    while (true) {
        sem_wait(&data->request_ready);
        if (strlen(data->func_name) == 0) {
            break;
        }
        function = dlsym(lib, data->func_name);
        data->result = function(data->value);
        sem_post(&data->response_ready);
    }
    sem_destroy(&data->request_ready);
    sem_destroy(&data->response_ready);

    munmap(data, sizeof(shared_data_t));
    close(fd);
    shm_unlink(shm_name);

    return 0;
}
