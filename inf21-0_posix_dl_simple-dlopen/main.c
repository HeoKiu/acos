#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char** argv) {
    if(argc < 3){
        fprintf(stderr, "Too few arguments");
        return 1;
    }
    char* filename = argv[1];
    char* symname = argv[2];

    double (*function)(double);

    void* lib = dlopen(filename, 0);

    function = dlsym(lib, symname);
    if(!function){
        perror("dlsym");
        return 1;
    }

    double value;
    while(scanf("%lf", &value) > 0){
        printf("%.3f\n", function(value));
    }

    return 0;
}
