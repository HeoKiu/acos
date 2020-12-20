#include <stdio.h>
#include <stdlib.h>


extern float dot_product(int N, const float *A, const float *B);


int main() {
    int N = 116;

    float *A = malloc(N * sizeof(float));
    float *B = malloc(N * sizeof(float));

    for (int i = 0; i < N; ++i) {
        A[i] = (float) i;
        B[i] = (float) i / (float)7;
    }

    printf("Dot product: %f\n", dot_product(N, A, B));

    float sum = 0;
    for(int i = 0; i < N; ++ i){
        sum += A[i] * B[i];
    }
    printf("Correct dot product: %f\n", sum);
    return 0;
}
