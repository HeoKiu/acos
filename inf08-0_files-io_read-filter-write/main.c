#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <fcntl.h>

const size_t buffer_size = 1000;


int read_bytes(int fd, char *buffer, int bytes_to_read) {

    for (size_t bytes_read = 0; bytes_read < bytes_to_read;) {
        ssize_t currently_read = read(fd, buffer + bytes_read, bytes_to_read - bytes_read);
        if (currently_read == -1) {
            return -1;
        }
        if (currently_read == 0) {
            return bytes_read;
        }
        bytes_read += currently_read;
    }
    return bytes_to_read;
}

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

void close_fd(int f1, int f2, int f3) {
    close(f1);
    close(f2);
    close(f3);
}

int main(int argc, char **args) {
    int input_file, output_digits, output_other;
    if (argc < 3) {
        return 3;
    }
    input_file = open(args[1], O_RDONLY);
    if (input_file == -1) {
        if (errno == ENOENT) {
            return 1;
        }
        return 3;
    }

    output_digits = open(args[2], O_CREAT | O_WRONLY, 0640);
    output_other = open(args[3], O_CREAT | O_WRONLY, 0640);

    if (output_digits == -1 || output_other == -1) {
        close_fd(input_file, output_digits, output_other);
        return 2;
    }

    char read_buffer[buffer_size];
    char digits_buffer[buffer_size];
    char other_buffer[buffer_size];

    int read_status;
    while (1) {
        read_status = read_bytes(input_file, read_buffer, buffer_size);
        if (read_status == -1) {
            close_fd(input_file, output_digits, output_other);
            return 3;
        }
        if (read_status == 0) {
            break;
        }
        int digits_pointer = 0;
        int other_pointer = 0;
        for (int i = 0; i < read_status; ++i) {
            if (read_buffer[i] >= '0' && read_buffer[i] <= '9') {
                digits_buffer[digits_pointer++] = read_buffer[i];
            } else {
                other_buffer[other_pointer++] = read_buffer[i];
            }
        }

        if (write_bytes(output_digits, digits_buffer, digits_pointer) == -1 ||
            write_bytes(output_other, other_buffer, other_pointer) == -1) {
            close_fd(input_file, output_digits, output_other);
            return 3;
        }
    }
    close_fd(input_file, output_digits, output_other);
    return 0;
}
