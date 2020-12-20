#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <fcntl.h>


int segment_size = 10000;

#define buffer_size 10000

char main_buffer[buffer_size];

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

int cmp(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}

void move(int fd, off_t offset) {
    lseek(fd, offset * sizeof(int), SEEK_SET);
}

void sort_file_segment(int fd, int buff_size) {
    int buffer[buff_size];
    off_t curr = lseek(fd, 0, SEEK_CUR);
    read_bytes(fd, (char *) buffer, sizeof(int) * buff_size);
    qsort(buffer, buff_size, sizeof(int), cmp);
    move(fd, curr / sizeof(int));
    write_bytes(fd, (char *) buffer, sizeof(int) * buff_size);
}

off_t file_size(int fd) {
    return lseek(fd, 0, SEEK_END) / sizeof(int);
}

int min(int a, int b) {
    if (a < b)return a;
    return b;
}

void copy_file_segment(int source_fd, int target_fd, int bytes_count) {
    while (bytes_count) {
        int step = min(buffer_size, bytes_count);
        read_bytes(source_fd, main_buffer, step);
        write_bytes(target_fd, main_buffer, step);
        bytes_count -= step;
    }
}

int merge(int *L, int n1, int *R, int n2, int *arr, int *left, int *right) {
    int j = 0, i = 0;
    int k = 0;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i++];
        } else {
            arr[k] = R[j++];
        }
        k++;
    }
    *left = i;
    *right = j;
    return k;
}


void merge_files(int fd1, int size1, int fd2, int size2, int fd_out) {
    move(fd1, 0);
    move(fd2, 0);
    int p1, p2;
    int buff1[buffer_size];
    int buff2[buffer_size];
    int buff_out[buffer_size * 2];
    while (size1 && size2) {
        p1 = min(size1, buffer_size);
        p2 = min(size2, buffer_size);

        read_bytes(fd1, (char *) buff1, p1 * sizeof(int));
        read_bytes(fd2, (char *) buff2, p2 * sizeof(int));
        int p1_, p2_;
        int merged = merge(buff1, p1, buff2, p2, buff_out, &p1_, &p2_);
        size1 -= p1_;
        size2 -= p2_;
        lseek(fd1, -(p1 - p1_) * sizeof(int), SEEK_CUR);
        lseek(fd2, -(p2 - p2_) * sizeof(int), SEEK_CUR);
        write_bytes(fd_out, (char *) buff_out, merged * sizeof(int));
    }
    if (size1 || size2) {
        int last_fd = fd1;
        int last_size = size1;
        if (size2) {
            last_fd = fd2;
            last_size = size2;
        }
        copy_file_segment(last_fd, fd_out, last_size * sizeof(int));
    }
}

void merge_sort(int fd, int left, int right, int t1, int t2) {
    if (right - left <= segment_size) {
        move(fd, left);
        sort_file_segment(fd, right - left);
    } else {
        int mid = left + (right - left) / 2;
        merge_sort(fd, left, mid, t1, t2);
        merge_sort(fd, mid, right, t1, t2);

        move(t1, 0);
        move(t2, 0);

        move(fd, left);
        copy_file_segment(fd, t1, sizeof(int) * (mid - left));
        move(fd, mid);
        copy_file_segment(fd, t2, sizeof(int) * (right - mid));

        move(fd, left);
        merge_files(t1, mid - left, t2, right - mid, fd);
    }
}

int main(int argc, char **argv) {
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        return 1;
    }
    int t1 = open("temp1", O_RDWR | O_CREAT, 0664);
    int t2 = open("temp2", O_RDWR | O_CREAT, 0664);
    int f_size = file_size(fd);

    merge_sort(fd, 0, f_size, t1, t2);

    close(fd);
    close(t1);
    close(t2);

    return 0;
}
