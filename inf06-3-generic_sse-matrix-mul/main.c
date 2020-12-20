#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <smmintrin.h>

/*
1 4
100000 100000 100000 100000
100000 100000 100000 100000
*/

int to_plain_idx(int i, int j, int N) {
    return i * N + j;
}

double dot_product(int N, float *A, float *B) {
    double result = 0;
    for (int i = 0; i + 4 <= N; i += 4) {
        __m128 xmm0 = _mm_loadu_ps(&A[i]);
        __m128 xmm1 = _mm_loadu_ps(&B[i]);
        xmm0 = _mm_dp_ps(xmm0, xmm1, 0b11110001);
        float res = _mm_cvtss_f32(xmm0);
        result += res;
    }
    for (int i = (N / 4) * 4; i < N; ++i) {
        result += A[i] * B[i];
    }
    return result;
}

double *matrix_product(int M, int N, float *A, float *B) {
    double *R = malloc(M * M * sizeof(double *));
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            R[to_plain_idx(i, j, M)] = dot_product(N, A + to_plain_idx(i, 0, N), B + to_plain_idx(j, 0, N));
        }
    }
    return R;
}


int main() {

    int M, N;
    scanf("%d %d", &M, &N);

    // Read two matrices A and B

    float *A = malloc(M * N * sizeof(float *));
    float *B = malloc(M * N * sizeof(float *));

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            scanf("%f", A + to_plain_idx(i, j, N));
        }
    }


    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < M; ++i) {
            scanf("%f", B + to_plain_idx(i, j, N));
        }
    }

    // R - product of matrices A and B
    double *R = matrix_product(M, N, A, B);

    free(A);
    free(B);


    // print matrix R

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            printf("%.5f ", R[to_plain_idx(i, j, M)]);
        }
        printf("\n");
    }
    free(R);


    return 0;
}