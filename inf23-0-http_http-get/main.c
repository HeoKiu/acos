#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

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

int tcp_connect(const struct sockaddr_in address){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == connect(sock, (const struct sockaddr*)&address, sizeof(address))){
        return -1;
    }
    return sock;
}

void http_get(int socket, char* server_name, char* path){
    char request[10000];

    char* headers_end = "\r\n\r\n";
    request[0] = '\0';
    sprintf(request + strlen(request), "GET %s HTTP/1.0\r\n", path);
    sprintf(request + strlen(request), "Host: %s%s", server_name, headers_end);

    write(socket, request, strlen(request));
    char mock_response[response_buffer_size];
    mock_response[0] = '\0';
    int n;
    char line[10000];
    int first = 1;
    while ((n = read(socket, line, sizeof(line) - 1)) > 0){
        line[n] = '\0';
        char* ptr = line;
        if(first){
            ptr = strstr(line, headers_end) + 4;
        }
        first = 0;
        printf("%s", ptr);
    }
}

int main(int argc, char ** argv) {
    if(argc < 3){
        fprintf(stderr, "Too few arguments");
        return 1;
    }

    char* server_name = argv[1];
    char* filepath = argv[2];

    char ip[20];

    if(-1 == hostname_to_ip(server_name, ip)){
        fprintf(stderr, "Fail to resolve host");
        return 1;
    }
    struct sockaddr_in addr = tcp_socket_addr(ip, 80);
    int server = tcp_connect(addr);

    http_get(server, server_name, filepath);

    return 0;
}
