#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define buffer_size 10000

int main()
{
    char buffer[buffer_size];
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        perror("gets");
        return 1;
    };
    char path[buffer_size] = "python3 -c \"print(", path_end[] = ")\"";
    strcat(path, buffer);
    strcat(path, path_end);

    char* argv[] = {"/bin/bash", "-c", path, NULL};

    if (-1 == execvp(argv[0], argv)) {
        perror("execvp");
        return 1;
    }
    return 0;
}
