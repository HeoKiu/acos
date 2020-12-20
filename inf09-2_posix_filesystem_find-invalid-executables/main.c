#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const int max_file_name = 5000;
#define buffer_size 6000

char magic_elf[] = {0x7F, 'E', 'L', 'F'};
char buffer[buffer_size];

void print_file(char *filename) {
    printf("%s\n", filename);
}

void move_begin(int fd) {
    lseek(fd, 0, SEEK_SET);
}

int is_correct_elf_file(int fd) {
    move_begin(fd);
    int read_bytes = read(fd, buffer, 4);
    if (read_bytes != 4) return 0;
    return strncmp(magic_elf, buffer, 4) == 0;
}

int is_file_truly_executable(char *filename);
int is_file_marked_executable(char *filename);

int is_correct_interpreted_file(int fd) {
    move_begin(fd);
    int read_bytes = read(fd, buffer, buffer_size);
    if (-1 == read_bytes) return 0;
    if (read_bytes < 3) return 0;
    if (buffer[0] != '#' || buffer[1] != '!') return 0;
    char interpreter_name[max_file_name];
    int i = 2;

    for (; i < read_bytes && buffer[i] != '\n' && buffer[i] != ' '; ++i) {
        interpreter_name[i - 2] = buffer[i];
    }
    interpreter_name[i - 2] = '\0';
    return is_file_marked_executable(interpreter_name);
}

int is_file_marked_executable(char *filename) {
    struct stat buff;
    int status = stat(filename, &buff);
    if (0 != status) return 0;
    return S_ISREG(buff.st_mode) && (buff.st_mode & S_IXUSR || buff.st_mode & S_IXGRP);
}

int is_file_executable(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (-1 == fd) return 0;
    int result = is_correct_elf_file(fd) || is_correct_interpreted_file(fd);
    close(fd);
    return result;
}

int is_file_truly_executable(char *filename) {
    return is_file_marked_executable(filename) && is_file_executable(filename);
}

int main() {
    char filename[max_file_name];

    while (fgets(filename, max_file_name, stdin)) {
        int len = strlen(filename);
        if(filename[len - 1] == '\n'){
            filename[len - 1] = '\0';
        }
        if (is_file_marked_executable(filename) && !is_file_executable(filename)) {
            print_file(filename);
        }
    }
    return 0;
}














