#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct {
    uint8_t octet[4];
    uint8_t type;
    uint8_t code;
    uint8_t sum[2];
} __attribute__((__packed__)) icmp_frame;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments");
        return 1;
    }
    in_addr_t ip_addr = inet_addr(argv[1]);
    int timeout = (int) strtol(argv[2], NULL, 10);
    int interval = (int) strtol(argv[3], NULL, 10);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    int fd = socket(AF_PACKET, SOCK_DGRAM, 0);
    if (-1 == fd) {
        perror("socket");
        return 1;
    }

    int num;
    while (scanf("%d", &num) > 0) {
        sendto(fd, &num, sizeof(num), 0, (struct sockaddr *) &addr, sizeof(addr));
        recvfrom(fd, &num, sizeof(num), 0, (struct sockaddr *) &addr, (socklen_t *) sizeof(addr));
        printf("%d\n", num);
    }

    return 0;
}
