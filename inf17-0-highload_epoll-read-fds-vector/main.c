#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void add_epoll(int efd, int fd, struct epoll_event *event) {
    set_non_blocking(fd);

    event->data.fd = fd;
    event->events = EPOLLIN;

    epoll_ctl(efd, EPOLL_CTL_ADD, fd, event);
}

size_t read_data_and_count(size_t N, int in[N]) {
    int epoll_fd = epoll_create(1);
    struct epoll_event entries[N];


    for (int i = 0; i < N; ++i) {
        add_epoll(epoll_fd, in[i], &entries[i]);
    }
    size_t files_complete = 0;
    int all_bytes = 0;
    while (files_complete < N) {
        struct epoll_event events[N];
        int count = epoll_wait(epoll_fd, events, N - files_complete, -1);

        for (int i = 0; i < count; ++i) {
            int fd = events[i].data.fd;
            int read_bytes = 0;
            char buffer[4096];
            if ((read_bytes = read(fd, buffer, sizeof(buffer))) <= 0) {
                close(fd);
                ++files_complete;
                continue;
            }
            all_bytes += read_bytes;
        }
    }
    close(epoll_fd);

    return all_bytes;
}