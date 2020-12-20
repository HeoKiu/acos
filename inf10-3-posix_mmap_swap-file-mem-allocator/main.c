#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <zconf.h>
#include <assert.h>
#include <string.h>

void myalloc_initialize(int fd /* открытый на R/W файловый дескриптор существующего файла */);

void *my_malloc(size_t size);

void my_free(void *ptr);

void myalloc_finalize();


//const size_t PAGE_SIZE = 4 * 1024; // Page size in bytes

size_t f_size;
typedef int32_t pointer_t;
pointer_t *mymalloc_ptr;


pointer_t align_bytes(size_t bytes_count) {
    if (!bytes_count) {
        return 0;
    }
    return (bytes_count - 1) / sizeof(pointer_t) + 1;
}

void print_file() {
    for (size_t i = 0; i < f_size; ++i) {
        printf("%d ", *(mymalloc_ptr + i));
    }
    printf("\n");
}

void myalloc_initialize(int fd) {
    f_size = lseek(fd, 0, SEEK_END);
    if (f_size < sizeof(pointer_t)) {
        fprintf(stderr, "Too small size of swap file");
    }
    // f_size should be multiple by sizeof(pointer_t)
    f_size /= sizeof(pointer_t);
    if (-1 == f_size) {
        perror("lseek");
        exit(1);
    }
    mymalloc_ptr = mmap(NULL, f_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == mymalloc_ptr) {
        perror("mmap");
        exit(1);
    }
    memset(mymalloc_ptr, 0, f_size * sizeof(pointer_t));
    *mymalloc_ptr = -(pointer_t) f_size;
}

ssize_t find_first_empty_gap(size_t size, size_t *gap_size) {
    if (!size) {
        return 0;
    }
    size_t max_gap = 0;
    for (int i = 0; i < f_size;) {
        pointer_t mem_shift = *(mymalloc_ptr + i);
        if (mem_shift > 0) {
            // Nearest memory is used
            max_gap = 0;
        } else if (mem_shift < 0) {
            // Nearest memory is free
            mem_shift *= -1;
            if (mem_shift + max_gap >= size) {
                *gap_size = mem_shift + max_gap;
                return i - max_gap;
            }
            max_gap += mem_shift;
        } else {
            fprintf(stderr, "Segmentation error");
            exit(1);
        }
        i += mem_shift;
    }
    return -1;
}

void myalloc_finalize() {
    if (NULL == mymalloc_ptr) {
        fprintf(stderr, "myalloc_initialize must be called before myalloc_finalize");
        exit(1);
    }
    munmap(mymalloc_ptr, f_size);
    mymalloc_ptr = NULL;
}

void *my_malloc(size_t size) {
    if (!size) {
        // Need ZERO bytes
        return NULL;
    }
    size = align_bytes(size);

    ++size; // size + pointer at the beginning
    size_t gap_size = 0;
    ssize_t gap_idx = find_first_empty_gap(size, &gap_size);
    if (-1 == gap_idx) {
        // Not enough empty space
        return NULL;
    }
    if (size < gap_size) {
        // Sign next memory part as free
        *(mymalloc_ptr + gap_idx + size) = -(gap_size - size);
    }
    *(mymalloc_ptr + gap_idx) = size;

    // User memory after pointer
    return mymalloc_ptr + gap_idx + 1;
}

void my_free(void *ptr) {
    pointer_t *aligned_ptr = ptr;
    if (ptr == NULL || aligned_ptr >= mymalloc_ptr + f_size || aligned_ptr < mymalloc_ptr) {
        fprintf(stderr, "Wrong ptr passed to my_free");
        exit(1);
    }
    --aligned_ptr; // Go to current memory pointer;
    if (*aligned_ptr <= 0) {
        // Space already free
        return;
    }
    pointer_t empty_space = *aligned_ptr;
    while (aligned_ptr + empty_space < mymalloc_ptr + f_size && *(aligned_ptr + empty_space) < 0) {
        empty_space += -*(aligned_ptr + empty_space);
    }
    *aligned_ptr = -empty_space;
}


//int main() {
//    // Тест в условиях большого количества места в файле
//
//    int swap_fd = open(".swap", O_RDWR | O_CREAT, 0664);
//    size_t file_size = 1000;
//    ftruncate(swap_fd, file_size);
//    myalloc_initialize(swap_fd);
//    close(swap_fd);
//
//    char *ptr = my_malloc(10);
//    *ptr = 4;
//    char p = *ptr;
//    assert(p == 4);
//    ptr += 9;
//    *ptr = 1;
//    assert(*ptr == 1);
//    *(ptr + 1) = 2;
//
//    char *ptr2 = my_malloc(2);
//    assert(ptr != ptr2);
//    *ptr2 = 8;
//    *(ptr2 + 1) = 32;
//    assert(*(ptr2 + 1) == 32);
//    assert(*ptr2 == 8);
//    assert(my_malloc(1e9) == NULL);
//    assert(my_malloc(file_size) == NULL);
//
//    assert(*ptr == 1);
//    assert(*(ptr + 1) == 2);
//    my_malloc(2);
//    my_malloc(2);
//    my_malloc(2);
//    my_malloc(2);
//    my_malloc(file_size);
//    my_free(ptr);
//    my_free(ptr2);
//
//    myalloc_finalize();
//
//    // Тестируем когда мало места
//    swap_fd = open(".swap", O_RDWR | O_CREAT, 0664);
//    file_size = 16; // 4*4
//    ftruncate(swap_fd, file_size);
//    myalloc_initialize(swap_fd);
//    close(swap_fd);
//
//    ptr = my_malloc(1); // Потратили 8 байт
//    ptr2 = my_malloc(1); // Потратили 8 байт
//    assert(ptr2 != NULL);
//    ptr2 = my_malloc(1); // Потратили 8 байт
//    assert(ptr2 == NULL);
//    ptr[0] = 14;
//    ptr[1] = 42;
//    assert(ptr[0] == 14);
//    assert(ptr[1] == 42);
//    my_free(ptr);
//    my_free(ptr);
//
//    myalloc_finalize();
//
//    return 0;
//}
