#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <zconf.h>

const int max_name = 10000;
const char* tempfile = "/tmp/144442212";

void make_response(int response_code)
{
    char response_status[10] = "OK";
    if (response_code != 200) {
        strcpy(response_status, "ERROR\n");
    }
    printf("HTTP/1.1 %d %s\n", response_code, response_status);
    fflush(stdout);
}

int main()
{
    char REQUEST_METHOD[10];
    char SCRIPT_NAME[max_name];
    char QUERY_STRING[max_name];
    char HTTP_HOST[max_name];
    char full_path[max_name];
    SCRIPT_NAME[0] = '.';

    scanf("%s ", REQUEST_METHOD);
    scanf("%s ", full_path);
    sscanf(full_path, "%[^?]s", &SCRIPT_NAME[1]);
    int have_query =
        sscanf(full_path + strlen(SCRIPT_NAME), "%s", QUERY_STRING);
    if (have_query != 1) {
        QUERY_STRING[0] = '\0';
    }
    scanf("HTTP/1.1\n");
    char header_name[max_name];
    char header_value[max_name];
    while (scanf("%s ", header_name) == 1) {
        scanf("%s\n", header_value);
        if (strcmp(header_name, "Host:") == 0) {
            strcpy(HTTP_HOST, header_value);
        }
    }

    //    printf("script_path: %d\n", have_query);
    //    printf("full_path: %s\n", full_path);
    //    printf("REQUEST_METHOD: %s\n", REQUEST_METHOD);
    //    printf("SCRIPT_NAME: %s\n", SCRIPT_NAME);
    //    printf("QUERY_STRING: %s\n", QUERY_STRING);
    //    printf("HTTP_HOST: %s\n", HTTP_HOST);

    int fd = open(SCRIPT_NAME, O_RDONLY);
    close(fd);

    if (-1 == fd) {
        make_response(404);
    } else if (access(SCRIPT_NAME, X_OK) == -1) {
        make_response(403);
    } else {
        int fork_status = fork();
        if (fork_status == 0) {
            make_response(200);
            char* env[5];
            for (int i = 0; i < 4; ++i) {
                env[i] = (char*)malloc(max_name);
            }
            env[4] = NULL;
            strcpy(env[0], "HTTP_HOST=");
            strcat(env[0], HTTP_HOST);
            strcpy(env[1], "QUERY_STRING=");
            strcat(env[1], QUERY_STRING);
            strcpy(env[2], "REQUEST_METHOD=");
            strcat(env[2], REQUEST_METHOD);
            strcpy(env[3], "SCRIPT_NAME=");
            strcat(env[3], SCRIPT_NAME + 1);
            char* argv[2];
            argv[1] = NULL;
            argv[0] = SCRIPT_NAME;
            execve(SCRIPT_NAME, argv, env);
        } else {
            waitpid(fork_status, NULL, 0);
        }
    }

    return 0;
}
