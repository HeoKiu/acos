#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const int max_filename = 6000;

int main(int argc, char **argv) {
    struct stat buff;
    off_t sum_files_size = 0;
    char filename[max_filename];
    while (fgets(filename, max_filename, stdin)) {
        int len = strlen(filename);
        if(filename[len - 1] == '\n'){
            filename[len - 1] = '\0';
        }
        int status = lstat(filename, &buff);
        if (0 == status && S_ISREG(buff.st_mode)) {
            sum_files_size += buff.st_size;
        }
    }
    printf("%ld", sum_files_size);

    return 0;
}
