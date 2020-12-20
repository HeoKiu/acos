#include <stdio.h>
#include <stdlib.h>

extern void summ(int N, const int *A, const int *B, int *R);


int main() {
    int N = 10;
    int *A = (int *) malloc(sizeof(int) * N);
    int *B = (int *) malloc(sizeof(int) * N);
    int *R = (int *) malloc(sizeof(int) * N);
    for (int i = 0; i < N; ++i) {
        A[i] = i;
        B[i] = 2;
    }
    summ(N, A, B, R);
    printf("R array:\n");

    for (int i = 0; i < N; ++i) {
        printf("%d ", R[i]);
    }
    printf("\n");
    free(A);
    free(B);
    free(R);
    return 0;
}
