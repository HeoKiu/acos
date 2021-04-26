#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_EVENTS 10
#define LISTEN_QUEUE_SIZE 100
#define CONNECTIONS_LIMIT 1000
#define REQUEST_BUFFER 4096

int must_stop_server = 0;

enum op {
    WAIT_WRITE,
    WAIT_READ
};

struct client_unit {
    char buffer[REQUEST_BUFFER];
    int buffer_len;
    int fd;
};

struct sockaddr_in tcp_socket_addr(char *ipv4_addr_str, unsigned int port) {
    in_addr_t ip_addr = inet_addr(ipv4_addr_str);
    struct sockaddr_in addr;
    struct in_addr in_addr;
    in_addr.s_addr = ip_addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = in_addr;
    addr.sin_port = htons(port);
    return addr;
}

void signal_handler(int signal) {
    printf("SIGNAL\n");
    if (signal == SIGINT || signal == SIGTERM || signal == SIGABRT) {
        printf("Catch stop signal\n");
        must_stop_server = 1;
    } else {
        printf("Ignore signal\n");
    }
    fflush(stdout);
}

void set_signal_handler() {
    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    sigaction(SIGPIPE, &signal_action, NULL);
    sigaction(SIGABRT, &signal_action, NULL);
}

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int build_server(struct sockaddr_in socket_addr, int listen_queue_size) {
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == bind(server_socket_fd,
                   (struct sockaddr *) &socket_addr,
                   sizeof(socket_addr))) {
        perror("bind");
        close(server_socket_fd);
        exit(1);
    }
    if (-1 == listen(server_socket_fd, listen_queue_size)) {
        perror("listen");
        close(server_socket_fd);
        exit(1);
    }
    set_non_blocking(server_socket_fd);
    return server_socket_fd;
}


int handle_read(struct client_unit *client) {
    int fd = client->fd;
    int read_bytes = read(fd, client->buffer, REQUEST_BUFFER);
    if (read_bytes > 0) {
        for (int i = 0; i < read_bytes; ++i) {
            client->buffer[i] = (char) toupper(client->buffer[i]);
        }
        client->buffer_len = read_bytes;
    }
    return read_bytes;
}

int handle_write(struct client_unit *client) {
    if (client->fd == -1) {
        return -1;
    }
    if (client->buffer_len > 0) {
        int bytes_wrote = write(client->fd, client->buffer, client->buffer_len);
        if (-1 == bytes_wrote) {
            return -1;
        }
        client->buffer_len -= bytes_wrote;
        return client->buffer_len;
    }
    return 0;
}

struct client_unit *initialize_connection(int fd) {
    struct client_unit *client = calloc(1, sizeof(struct client_unit));
    client->fd = fd;
    client->buffer_len = 0;
    return client;
}

void free_client(struct client_unit *client){
    close(client->fd);
    free(client);
    printf("Delete client\n");
    fflush(stdout);
}

void epoll_add_write(int epollfd, int fd, struct epoll_event* event){
    event->events |= EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, event);
}

void epoll_del_write(int epollfd, int fd, struct epoll_event* event){
    event->events &= ~EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, event);
}

void run_server(int server_socket) {
    int conn_sock, nfds, epollfd;
    int current_connection_count = 0;
    struct epoll_event event, events[MAX_EVENTS];

    // Create epoll with server socket
    epollfd = epoll_create1(0);
    event.data.ptr = initialize_connection(server_socket);
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socket, &event);

    sigset_t sigset;
    sigfillset(&sigset);
    sigdelset(&sigset, SIGINT);
    sigdelset(&sigset, SIGTERM);
    printf("Begin wait\n");
    for (;;) {
        printf("Wait (current connections: %d)\n", current_connection_count);
        nfds = epoll_pwait(epollfd, events, MAX_EVENTS, -1, &sigset);
        printf("nfds = %d\n", nfds);
        if (-1 == nfds) {
            perror("epoll_pwait");
            break;
        }
        for (int n = 0; n < nfds; ++n) {
            printf("Get %d connections\n", nfds);
            struct sockaddr_in client_addr;
            socklen_t client_addr_len;
            struct client_unit *conn = events[n].data.ptr;
            if (conn->fd == server_socket) {
                printf("Accept new client\n");
                conn_sock = accept(server_socket,
                                   (struct sockaddr *) &client_addr, &client_addr_len);
                set_non_blocking(conn_sock);
                if (current_connection_count >= CONNECTIONS_LIMIT) {
                    fprintf(stderr, "Too many connections: %d\n", CONNECTIONS_LIMIT);
                    close(conn_sock);
                    continue;
                }
                ++current_connection_count;
                struct client_unit *client = initialize_connection(conn_sock);
                event.data.ptr = client;
                event.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &event);
            } else {
                if (events[n].events & EPOLLIN) {
                    printf("Ready to read\n");
                    int handling_status = handle_read(conn);
                    if (-1 == handling_status) {
                        free_client(conn);
                        --current_connection_count;
                        continue;
                    }
                    if(handling_status > 0){
                        epoll_add_write(epollfd, conn->fd, &events[n]);
                    }
                }
                if (events[n].events & EPOLLOUT) {
                    printf("Ready to write\n");
                    int handling_status = handle_write(conn);
                    if (-1 == handling_status) {
                        free_client(conn);
                        --current_connection_count;
                        continue;
                    }
                    if(conn->buffer_len == 0){
                        epoll_del_write(epollfd, conn->fd, &events[n]);
                    }
                } else {
                    free_client(conn);
                    --current_connection_count;
                }
            }
        }
    }
}

int hostname_to_ip(char *hostname, char *ip) {
    struct hostent *host_resolved;
    struct in_addr **addr_list;

    if ((host_resolved = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return -1;
    }

    addr_list = (struct in_addr **) host_resolved->h_addr_list;
    if (host_resolved->h_length == 0) {
        return -1;
    }
    strcpy(ip, inet_ntoa(*addr_list[0]));

    return 0;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return 1;
    }
    printf("My pid: %d\n", getpid());
    set_signal_handler();

    char *port_str = argv[1];

    unsigned int port = (int) strtol(port_str, NULL, 10);

    char ip_str[100];
    if (-1 == hostname_to_ip("localhost", ip_str)) {
        return 1;
    }
    printf("Listen on ip: %s\n", ip_str);
    struct sockaddr_in addr = tcp_socket_addr(ip_str, port);
    printf("Run server on port: %s\n", port_str);

    int server_fd = build_server(addr, LISTEN_QUEUE_SIZE);

    run_server(server_fd);

    printf("Close server\n");
    close(server_fd);

    return 0;
}