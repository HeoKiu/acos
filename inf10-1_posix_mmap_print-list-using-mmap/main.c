#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <zconf.h>
#include <sys/mman.h>


int write_bytes(int fd, char *buffer, int bytes_to_write) {
    for (size_t bytes_written = 0; bytes_written < bytes_to_write;) {
        ssize_t currently_written = write(fd, buffer + bytes_written, bytes_to_write - bytes_written);
        if (currently_written == -1) {
            return -1;
        }
        bytes_written += currently_written;
    }
    return bytes_to_write;
}


typedef struct {
    int value;
    uint32_t next_pointer;
} Item;

Item getItem(char* item_pos) {
    void* item_p = item_pos;
    Item* item = item_p;
    return *item;
}

char digits[20];
const int buffer_size = 1000;


uint32_t print(char *buffer, int pointer, int value) {
    if (value < 0) {
        buffer[pointer++] = '-';
        value *= -1;
    }
    int dig_pointer = 0;
    if (!value) {
        digits[dig_pointer++] = '0';
    }
    while (value) {
        digits[dig_pointer++] = '0' + value % 10;
        value /= 10;
    }
    for (int i = 0; i < dig_pointer; ++i) {
        buffer[pointer++] = digits[dig_pointer - 1 - i];
    }
    buffer[pointer++] = ' ';
    return pointer;
}

int main(int argc, char **argv) {
    char buffer[buffer_size];
    uint32_t buffer_pointer = 0;
    uint32_t next_pointer = 0;

    int file = open(argv[1], O_RDONLY);
    if (file == -1) {
        return 1;
    }
    off_t off = lseek(file, 0, SEEK_END);

    char *file_pointer = mmap(NULL, off, PROT_READ, MAP_PRIVATE, file, 0);

    if (off > 0) {
        Item item;
        do {
            item = getItem(file_pointer + next_pointer);
            next_pointer = item.next_pointer;
            buffer_pointer = print(buffer, buffer_pointer, item.value);
            if (buffer_pointer > buffer_size - 30) {
                write_bytes(STDOUT_FILENO, buffer, buffer_pointer);
                buffer_pointer = 0;
            }
        } while (next_pointer != 0);
        if (buffer_pointer > 0) {
            write_bytes(STDOUT_FILENO, buffer, buffer_pointer);
        }
    }
    munmap(file_pointer, off);
    close(file);

    return 0;
}