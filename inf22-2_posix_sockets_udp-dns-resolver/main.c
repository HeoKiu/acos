#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <assert.h>

volatile sig_atomic_t signal_received = 0;

#define DNS_IP "8.8.8.8"

typedef struct {
    uint16_t type;
    uint16_t class;
} __attribute__((__packed__)) dns_question_flags_t;

typedef struct {
    char domain[1000];
    dns_question_flags_t flags;
} dns_question_t;

typedef struct {
    uint16_t name_ptr;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_length;
    uint32_t ip;
} __attribute__((__packed__)) dns_answer_t;

typedef struct {
    uint16_t transaction_id;
    uint8_t flags[2];
    uint16_t questions_count;
    uint16_t answers_count;
    uint16_t authority;
    uint16_t additional;
} __attribute__((__packed__)) dns_header_t;

typedef struct {
    dns_header_t header;
    dns_question_t *questions;
    dns_answer_t *answers;
} dns_packet_t;

void build_dns_request(dns_packet_t *pkt, dns_question_t *questions, int questions_count) {
    memset(pkt, 0, sizeof(dns_packet_t));
    pkt->header.flags[0] = 1;
    pkt->header.questions_count = htons(questions_count);
    pkt->questions = questions;
}

int represent_dns_packet(dns_packet_t *pkt, char *buffer) {
    uint8_t elapsed = 0;
    memcpy(buffer, &pkt->header, sizeof(pkt->header));
    elapsed += sizeof(pkt->header);
    for (int i = 0; i < ntohs(pkt->header.questions_count); ++i) {
        sprintf(buffer + elapsed, "%s", pkt->questions[i].domain);
        uint16_t domain_len = strlen(pkt->questions[i].domain);
        elapsed += domain_len + 1;
        memcpy(buffer + elapsed, &pkt->questions[i].flags, sizeof(pkt->questions[i].flags));
        elapsed += sizeof(pkt->questions[i].flags);
    }
    return elapsed;
}

void parse_dns_packet(dns_packet_t *pkt, char *buffer) {
    uint32_t elapsed = 0;
    memcpy(&pkt->header, buffer, sizeof(pkt->header));
    elapsed += sizeof(pkt->header);
    // Skip questions
    for (int i = 0; i < ntohs(pkt->header.questions_count); ++i) {
        // Skip question
        elapsed += strlen(buffer + elapsed) + 1;
        elapsed += sizeof(dns_question_flags_t);
    }
    // Skip answer domain name
    for (int i = 0; i < ntohs(pkt->header.answers_count); ++i) {
        memcpy(&pkt->answers[i], buffer + elapsed, sizeof(dns_answer_t));
        elapsed += ntohs(pkt->answers[i].data_length) - 4;
        elapsed += sizeof(dns_answer_t);
    }
}

void build_dns_question(dns_question_t *question, char *domain) {
    memset(question->domain, 0, sizeof(question->domain));
    question->flags.type = htons(1);
    question->flags.class = htons(1);

    char word[256];
    int passed = 0;
    uint8_t word_len = 0;
    int domain_len = strlen(domain);

    for (int i = 0; i <= domain_len; ++i) {
        if (i == domain_len || domain[i] == '.') {
            *(question->domain + strlen(question->domain)) = word_len;
            passed += word_len + 1;
            strncpy(question->domain + strlen(question->domain), word, word_len);
            word_len = 0;
        }else{
            ++word_len;
            word[i - passed] = domain[i];
        }
    }
}

struct sockaddr_in get_socket_addr(char *ip) {
    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(53);
    target_addr.sin_addr.s_addr = inet_addr(ip);
    return target_addr;
}

uint32_t get_ip(dns_packet_t* pkt){
    for (int i = 0; i < ntohs(pkt->header.answers_count); ++i) {
        if(ntohs(pkt->answers[i].type) == 1){
            return pkt->answers[i].ip;
        }
    }
    return 0;
}

void ip_to_str(char* ip_str, uint32_t ip_num){
    struct in_addr ip_addr;
    ip_addr.s_addr = ip_num;
    char* ip_str_tmp = inet_ntoa(ip_addr);
    memcpy(ip_str, ip_str_tmp, strlen(ip_str_tmp));
    ip_str[strlen(ip_str_tmp)] = 0;
}

void resolve_domain(char *domain, char *ip) {
    assert(domain != NULL);
    // AF_INET - абстракция уровня ip (заголовок ip пакета включен)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    dns_packet_t dns_request;
    dns_question_t questions[1];
    build_dns_request(&dns_request, questions, 1);
    build_dns_question(&questions[0], domain);

    char buffer[1000];
    int request_size = represent_dns_packet(&dns_request, buffer);

    struct sockaddr_in dns_addr = get_socket_addr(DNS_IP);
    // Send DNS request
    sendto(sock, &buffer, request_size, 0, (struct sockaddr *) &dns_addr, sizeof(dns_addr));

    // Receive DNS reply
    socklen_t addrlen;
    recvfrom(sock, &buffer, sizeof(buffer), 0, (struct sockaddr *) &dns_addr, &addrlen);

    // Подготовили структуры для ответа
    dns_packet_t dns_reply;
    dns_answer_t answers[10];
    dns_reply.answers = answers;

    parse_dns_packet(&dns_reply, buffer);

    uint32_t ip_num = get_ip(&dns_reply);
    ip_to_str(ip, ip_num);
}

int main() {
    char domain_name[1000];
    char ip[1000];

    while (scanf("%s", domain_name) > 0) {
        resolve_domain(domain_name, ip);
        printf("%s\n", ip);
    }
    return 0;
}