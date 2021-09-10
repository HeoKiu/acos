#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define response_buffer_size 1000000

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

int tcp_connect(const struct sockaddr_in address) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connect(sock, (const struct sockaddr *) &address, sizeof(address))) {
        return -1;
    }
    return sock;
}

void print_response(int socket) {
    int n;
    char line[10000];
    int first = 1;
    while ((n = read(socket, line, sizeof(line) - 1)) > 0) {
        line[n] = '\0';
        char *ptr = line;
        if (first && strncmp(line, "HTTP", 4) == 0) {
            ptr = strstr(line, "\r\n\r\n") + 4;
        }
        first = 0;
        printf("%s", ptr);
    }
}

int write_all(int socket, char *buffer, int size) {
    int n, wrote = 0;
    while ((n = write(socket, buffer + wrote, size - wrote)) > 0) {
        wrote += n;
        if (wrote == size) {
            break;
        }
    }
    return wrote;
}

void http_post(int socket, char *server_name, char *path, int file_fd) {
    char request[10000];

    struct stat file_stat;
    fstat(file_fd, &file_stat);

    request[0] = '\0';
    sprintf(request + strlen(request), "POST %s HTTP/1.0\r\n", path);
    sprintf(request + strlen(request), "Host: %s\r\n", server_name);
    sprintf(request + strlen(request), "Content-Length: %zu\r\n", file_stat.st_size);
    // End of headers
    sprintf(request + strlen(request), "\r\n");

    write_all(socket, request, strlen(request));

    char buffer[1000];
    int n;
    while ((n = read(file_fd, buffer, sizeof(buffer))) > 0) {
        write_all(socket, buffer, n);
    }

    print_response(socket);
    close(socket);
    close(file_fd);
}

int open_file(char *filename) {
    int fd = open(filename, 0666);
    return fd;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }

    char *server_name = argv[1];
    char *script_path = argv[2];
    char *filepath = argv[3];

    char ip[20];

    if (-1 == hostname_to_ip(server_name, ip)) {
        fprintf(stderr, "Fail to resolve host");
        return 1;
    }
    struct sockaddr_in addr = tcp_socket_addr(ip, 80);
    int server = tcp_connect(addr);

    http_post(server, server_name, script_path, open_file(filepath));

    return 0;
}