#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        perror("Too few arguments");
        return 1;
    }
    if (-1 == mkfifo(argv[1], 0666)) {
        return 1;
    }
    int target_pid;
    scanf("%d", &target_pid);

    kill(target_pid, SIGHUP);

    int fifo_fd = open(argv[1], O_WRONLY);
    if (-1 == fifo_fd) {
        perror("Can't open fifo");
        return 1;
    }
    int N = (int) strtol(argv[2], NULL, 10);

    int max_number_len = 10;

    char number[max_number_len];
    for (int i = 0; i <= N; ++i) {
        sprintf(number, "%d ", i);
        int wrote = 0;
        int number_len = (int) strlen(number);
        while (wrote < number_len) {
            int curr_wrote = 0;
            if (-1 == (curr_wrote = write(fifo_fd, number + wrote, number_len - wrote))) {
                close(fifo_fd);
                printf("%d\n", i);
                return 0;
            }
            wrote += curr_wrote;
        }
    }
    close(fifo_fd);
    printf("%d\n", N + 1);
    return 0;
}
