#include <stdio.h>
#include <fcntl.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

char str[20];

void write_line(char *begin, int step, int cell_size, int count, int begin_value) {
    for (int i = 0; i < count; ++i) {
        sprintf(str, "%d", i + begin_value);
        int len = (int) strlen(str);
        strncpy(begin + i * step + (cell_size - len), str, len);
    }
}

void write_spiral(char *file, int f_size, int N, int W) {
    for (int i = 0; i < f_size; ++i) {
        *(file + i) = ' ';
        if (i % (N * W + 1) == N * W) {
            *(file + i) = '\n';
        }
    }
    int value = 1;
    int file_width = N * W + 1;
    for (int line_len = N; line_len > 0; line_len -= 2) {
        write_line(file, W, W, line_len, value);
        write_line(file + (line_len - 1) * W + file_width, file_width, W, line_len - 1, value + line_len);
        write_line(file + (line_len - 2) * W + file_width * (line_len - 1), -W, W, line_len - 1,
                   value + line_len * 2 - 1);
        write_line(file + file_width * (line_len - 1), -file_width, W, line_len - 1, value + line_len * 3 - 3);
        file += file_width + W;
        value += line_len * 4 - 4;
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "not enough arguments\n");
        return 1;
    }
    int fd = open(argv[1], O_RDWR | O_CREAT, 0664);
    if (-1 == fd) {
        return 2;
    }
    int N = (int) strtol(argv[2], NULL, 10);
    int W = (int) strtol(argv[3], NULL, 10);
    off_t f_size = N * N * W + N;
    ftruncate(fd, f_size);
    char *file = mmap(NULL, f_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) {
        perror("mmap");
        return 3;
    }

    write_spiral(file, f_size, N, W);

    munmap(file, f_size);
    close(fd);

    return 0;
}
