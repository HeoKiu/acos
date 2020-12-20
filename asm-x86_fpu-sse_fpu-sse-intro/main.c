#include <stdio.h>
#include <stdlib.h>


//calc:
//push ebp
//mov	ebp, esp
//
//fld qward ptr [ebp+8]
////fadd qward ptr [ebp+16]
//
////fild dward ptr [ebp+32]
//// fadd qward ptr [ebp+24]
//
//// fdivp st(1), st(0)
//
//leave
//        ret

extern double
calc(double A, double B, double C, int D);

extern void
vector_sum(int N, const float *A, const float *B, float *R);


int main() {
    printf("(2 + 3) / (4 + 0) = %f\n", calc(2, 3, 4, 0));
    printf("(1.5 + 2) / (1.1 + 3) = %f\n", calc(1.5, 2, 1.1, 3));

    int N = 4;
    float *A = malloc(N * sizeof(float));
    float *B = malloc(N * sizeof(float));
    float *R = malloc(N * sizeof(float));
    for (int i = 0; i < N; ++i) {
        A[i] = (float) i / 2;
        B[i] = (float) i / 2 * 2;
        printf("R[%d] should be %f\n", i, A[i] + B[i]);
    }
    vector_sum(N, A, B, R);
    printf("\nvector_sum executed\n");
    for (int i = 0; i < N; ++i) {
        printf("R[%d] is %f\n", i, R[i]);
    }

    return 0;
}
