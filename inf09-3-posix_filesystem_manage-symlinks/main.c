#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zconf.h>

const int max_name_len = 1000;

int main() {

    char filename[max_name_len];
    char absolute_path[max_name_len];

    char* prefix = "link_to_";
    char name_with_prefix[max_name_len];

    while(EOF != scanf("%s", filename)){
        struct stat buff;
        int status = lstat(filename, &buff);
        if(-1 == status){
            continue;
        }
        if(S_ISLNK(buff.st_mode)){
            realpath(filename, absolute_path);
            printf("%s\n", absolute_path);
        }else if(S_ISREG(buff.st_mode)){
            strcpy(name_with_prefix, prefix);
            strcat(name_with_prefix, filename);
            symlink(filename, name_with_prefix);
        }
    }
    return 0;
}
