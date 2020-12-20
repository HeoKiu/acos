#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <zconf.h>
#include <string.h>

int main(int argc, char **argv) {

    int fd = open(argv[1], O_RDONLY);
    off_t f_size = lseek(fd, 0, SEEK_END);
    char *file = mmap(NULL, f_size, PROT_READ, MAP_PRIVATE, fd, 0);

    char *str = argv[2];
    int str_len = (int)strlen(str);

    for (int i = 0; i < (int)f_size - str_len; ++i) {
        int j = 0;
        while (j < str_len && str[j] == *(file + i + j)) { ++j; }
        if(j == str_len){
            printf("%d ", i);
        }
    }
    munmap(file, f_size);
    close(fd);

    return 0;
}
