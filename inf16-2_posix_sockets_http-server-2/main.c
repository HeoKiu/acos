#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <wait.h>

const int LISTEN_QUEUE_SIZE = 20;
const int MAX_FILENAME_LEN = 300;
const int MAX_FILEPATH_LEN = 500;
const int MAX_REQUEST_LEN = 5000;

int must_stop_server = 0;
pid_t current_child_pid = -1;

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
    return server_socket_fd;
}


int read_request(int socket_fd, char *request_text) {
    int buffer_size = 512;
    char buffer[buffer_size + 1];
    int curr_bytes_read;
    int bytes_read = 0;
    while ((curr_bytes_read = recv(socket_fd, buffer, buffer_size, 0)) > 0) {
        if (bytes_read + curr_bytes_read > MAX_REQUEST_LEN - 2) {
            fprintf(stderr, "Too big request body: %d bytes\n", bytes_read + curr_bytes_read);
            return -1;
        }
        buffer[curr_bytes_read] = '\0';
        strncpy(request_text + bytes_read, buffer, curr_bytes_read);
        bytes_read += curr_bytes_read;
        if (bytes_read >= 4 && strncmp(request_text + bytes_read - 4, "\r\n\r\n", 4) == 0) {
            break;
        }
    }
    if (curr_bytes_read == -1) {
        perror("Read request");
        return -1;
    }
    request_text[bytes_read] = '\0';
    return bytes_read;
}

int read_request_head(char *request_buffer, char *filename) {
    char *token = strtok(request_buffer, " ");
    if (token == NULL || strncmp("GET", token, 3) != 0) {
        fprintf(stderr, "Wrong request format. Must begin with GET\n");
        return -1;
    }
    char *filename_ptr = strtok(NULL, " ");
    if (filename_ptr == NULL || strlen(filename_ptr) <= 1) {
        fprintf(stderr, "Wrong filename passed\n");
        return -1;
    }
    char *protocol = strtok(NULL, " \r\n");
    if (protocol == NULL || strncmp("HTTP/1.1", protocol, 8) != 0) {
        fprintf(stderr, "Wrong protocol provided, must be HTTP/1.1\n");
        return -1;
    }
    strcpy(filename, filename_ptr);
    return 0;
}

int write_all(int fd, char *text, int text_len) {
    int curr_written;
    int written = 0;
    while ((curr_written = write(fd, text + written, text_len - written)) > 0) {
        written += curr_written;
        if (written == text_len) {
            return text_len;
        }
    }
    if (curr_written == 0) {
        return text_len;
    }
    return -1;
}

int copy_file(int dest_fd, int src_fd) {
    int buffer_size = 1024;
    char buffer[buffer_size];
    int curr_read;
    int read_bytes = 0;
    while ((curr_read = read(src_fd, buffer, buffer_size)) > 0) {
        read_bytes += curr_read;
        if (-1 == write_all(dest_fd, buffer, curr_read)) {
            fprintf(stderr, "Error writing in client_socket\n");
            return -1;
        }
    }
    if (curr_read == 0) {
        return 0;
    }
    fprintf(stderr, "Error reading from file\n");
    return -1;
}

int append_text(int fd, char *text) {
    return write_all(fd, text, (int) strlen(text));
}

int execute_file(char *filepath) {
    int pipes[2];
    if (-1 == pipe(pipes)) {
        perror("pipe");
        return -1;
    }

    pid_t child_pid = fork();
    if (child_pid == 0) {
        close(pipes[0]);
        dup2(pipes[1], 1);
        execl(filepath, filepath, NULL, NULL);
        perror("Exec");
        return -1;
    }
    close(pipes[1]);
    current_child_pid = child_pid;
    return pipes[0];
}

int make_response(char *base_path, char *filename, int response_fd) {
    struct stat file_info;

    char filepath[MAX_FILEPATH_LEN];
    if (filename[0] == '/') {
        ++filename;
    }
    if (strlen(filename) < 1) {
        fprintf(stderr, "Wrong filename\n");
        return -1;
    }
    sprintf(filepath, "%s/%s", base_path, filename);

    int stat_status = stat(filepath, &file_info);
    int stat_errno = errno;

    append_text(response_fd, "HTTP/1.1");
    if (-1 == stat_status) {
        if (ENOENT == stat_errno) {
            append_text(response_fd, " 404 Not Found\r\n");
            return 0;
        }
        append_text(response_fd, " 418 I’m a teapot\r\n");
        return 0;
    }
    if (!(file_info.st_mode & S_IRUSR)) {
        append_text(response_fd, " 403 Forbidden\r\n");
        return 0;
    }
    append_text(response_fd, " 200 OK\r\n");

    if (file_info.st_mode & S_IXUSR) {
        // File executable. Exec and print output
        int output_fd = execute_file(filepath);
        if (-1 == output_fd) {
            return -1;
        }
        append_text(response_fd, "\r\n");
        int write_file_status = copy_file(response_fd, output_fd);
        close(output_fd);
        return write_file_status;
    } else {
        // Return file as-is
        append_text(response_fd, "Content-Length: ");

        char file_size_str[20];
        int file_size = file_info.st_size;
        sprintf(file_size_str, "%d", file_size);
        append_text(response_fd, file_size_str);
        append_text(response_fd, "\r\n");
        append_text(response_fd, "\r\n");

        int file_fd = open(filepath, O_RDONLY);
        int write_file_status = copy_file(response_fd, file_fd);
        close(file_fd);
        return write_file_status;
    }
}

int handle_request(int socket_fd, char *base_path) {
    char filename[MAX_FILENAME_LEN];
    char request_text[MAX_REQUEST_LEN];

    int request_size = read_request(socket_fd, request_text);
    if (request_size <= 0) {
        return -1;
    }

    if (-1 == read_request_head(request_text, filename)) {
        return -1;
    }
    printf("Filename: %s\n", filename);
    if (-1 == make_response(base_path, filename, socket_fd)) {
        return -1;
    }
    return 0;
}

void run_file_server(int server_socket_fd, char *base_path) {
    while (1) {
        if (must_stop_server) {
            fprintf(stderr, "STOP\n");
            break;
        }
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;

        int client_sock_fd = accept(server_socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (-1 == client_sock_fd) {
            perror("Accept request");
            continue;
        }
        printf("New request\n");

        int request_status = handle_request(client_sock_fd, base_path);
        if (request_status == -1) {
            printf("An error has occurred while handling request\n\n");
        } else {
            printf("Successful request\n\n");
        }

        close(client_sock_fd);
    }
}


void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM || signal == SIGABRT) {
        if (current_child_pid > 0) {
            kill(current_child_pid, SIGKILL);
            wait(NULL);
            current_child_pid = -1;
        }
        must_stop_server = 1;
    }
    if (signal == SIGCHLD) {
        if (current_child_pid > 0) {
            waitpid(current_child_pid, NULL, 0);
            current_child_pid = -1;
        }
    }
}

void set_signal_handler() {
    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    sigaction(SIGPIPE, &signal_action, NULL);
    sigaction(SIGABRT, &signal_action, NULL);
    sigaction(SIGCHLD, &signal_action, NULL);
}

char *normalize_base_path(char *base_path) {
    if (base_path[strlen(base_path) - 1] == '/') {
        base_path[strlen(base_path) - 1] = '\0';
    }
    return base_path;
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
    if (argc < 3) {
        fprintf(stderr, "Too few arguments. Example: <program name> <port> <base dir>\n");
        return 1;
    }
    set_signal_handler();

    char *port_str = argv[1];
    char *base_path = argv[2];
    base_path = normalize_base_path(base_path);

    unsigned int port = (int) strtol(port_str, NULL, 10);

    char ip_str[100];
    if (-1 == hostname_to_ip("localhost", ip_str)) {
        return 1;
    }
    printf("Ip is %s\n", ip_str);
    // Obvious ip is 127.0.0.1, but localhost cat be changed manually to another
    struct sockaddr_in addr = tcp_socket_addr(ip_str, port);

    printf("Run server on port: %s\n", port_str);
    int server_fd = build_server(addr, LISTEN_QUEUE_SIZE);

    run_file_server(server_fd, base_path);

    close(server_fd);

    return 0;
}
