#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **args) {
    float x;
    char y[10], *end;

    scanf("%f%s", &x, y);

    double result = x + (double) strtol(y, &end, 16) + (double) strtol(args[1], &end, 27);

    printf("%.3f", result);
    return 0;
}
