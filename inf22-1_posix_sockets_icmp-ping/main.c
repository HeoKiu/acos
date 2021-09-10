#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdbool.h>
#include <sys/select.h>
#include <netinet/ip_icmp.h>
#include <assert.h>

volatile sig_atomic_t signal_received = 0;

typedef struct {
    uint8_t type;
    uint8_t code;
    uint32_t checksum;
    uint8_t data[4];
} __attribute__((__packed__)) icmp_packet_t;


void sig_handler(int signum) {
    // Handle SIGALARM. Do nothing

    if(signum == SIGALRM){
        signal_received = true;
    }
}

static ushort checksum(void *b, int len) {
    ushort *buf = b;
    uint sum = 0;
    ushort result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *) buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


void make_ping_icmp_packet(icmp_packet_t *ping_pkt) {
    ping_pkt->type = 8;
    ping_pkt->code = 0;
    ping_pkt->checksum = 0;
    memset(ping_pkt->data, 1, sizeof(ping_pkt->data));
    ping_pkt->checksum = checksum(ping_pkt, sizeof(icmp_packet_t));
}


bool ping(const char *ip) {
    assert(ip != NULL);

    // AF_INET - абстракция уровня ip (заголовок ip пакета включен)
    // SOCK_RAW - данные пишем сами, tcp/udp не используется
    // IPPROTO_ICMP - в поле протокола в ip пакете указан 1 (icmp)
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Socket");
        return false;
    }

    // Адрес получателя. Ему отправляем пакет
    struct in_addr ip4_addr;
    assert(inet_aton(ip, &ip4_addr) != 0);
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr = ip4_addr;

    socklen_t socklen = sizeof(struct sockaddr_in);
    icmp_packet_t icmp_request;
    make_ping_icmp_packet(&icmp_request);
    if (sendto(sock, &icmp_request, sizeof(icmp_request), 0, (struct sockaddr *) &target_addr, socklen) <= 0) {
        perror("Sendto");
        close(sock);
        return false;
    }

    icmp_packet_t icmp_reply;

    if (recvfrom(sock, &icmp_reply, sizeof(icmp_reply), 0, (struct sockaddr *) &target_addr, &socklen) <= 0) {
        return false;
    }
    close(sock);
    return true;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }

    struct sigaction action = {
            .sa_mask = 0,
            .sa_flags = 0,
            .sa_handler = sig_handler
    };
    sigaction(SIGALRM, &action, NULL);

    char *ipv4 = argv[1];
    int timeout = (int) strtol(argv[2], NULL, 10);
    int interval = (int) strtol(argv[3], NULL, 10);

    alarm(timeout);

    // Wait signal SIGALARM to stop
    int success_responses = 0;
    while (!signal_received) {
        success_responses += ping(ipv4);
        usleep(interval);
    }
    printf("%d\n", success_responses);
}