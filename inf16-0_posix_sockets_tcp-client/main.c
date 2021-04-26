#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        perror("Too few arguments");
        return 1;
    }
    in_addr_t ip_addr = inet_addr(argv[1]);
    in_port_t port = htons(strtol(argv[2], NULL, 10));

    struct sockaddr_in addr;
    struct in_addr in_addr;
    in_addr.s_addr = ip_addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = in_addr;
    addr.sin_port = port;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr))) {
        perror("Fail to connect");
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    int value = 0, receive_value = 0;
    while (scanf("%d", &value) > 0) {
        if (write(socket_fd, &value, sizeof(value)) <= 0) {
            break;
        }
        if (read(socket_fd, &receive_value, sizeof(receive_value)) <= 0) {
            break;
        }
        printf("%d\n", receive_value);
        fflush(stdout);
    }

    return 0;
}
