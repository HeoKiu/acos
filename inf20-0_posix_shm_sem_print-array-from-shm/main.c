#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    char sem_name[1000];
    char mem_block[1000];
    int n;
    scanf("%s %s %d", sem_name, mem_block, &n);

    sem_t *is_data_ready = sem_open(sem_name, O_RDONLY);
    sem_wait(is_data_ready);

    int fd = shm_open(mem_block, O_RDONLY, 0);
    char *ptr = mmap(NULL, sizeof(int) * n, PROT_READ, MAP_SHARED, fd, 0);
    for (int i = 0; i < n; ++i) {
        printf("%d\n", *ptr);
        ptr += sizeof(int);
    }

    sem_close(is_data_ready);
    munmap(ptr, sizeof(int) * n);
    close(fd);

    return 0;
}
