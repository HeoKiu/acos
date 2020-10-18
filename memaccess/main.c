#include <stdio.h>

extern int summ(int x0, int N, int *X);


int main() {
    int values[] = {1};

    printf("%d", summ(0, 0, values));
    return 0;
}
