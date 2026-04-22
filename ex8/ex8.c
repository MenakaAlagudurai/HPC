#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000

int main() {
    double *A = (double *)malloc(N * sizeof(double));
    double *B = (double *)malloc(N * sizeof(double));
    double *C = (double *)malloc(N * sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = 2 * i;
    }

    // Start timing
    double start_time = omp_get_wtime();

    // Parallel addition
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        C[i] = A[i] + B[i];
    }

    // End timing
    double end_time = omp_get_wtime();
    double duration = end_time - start_time;

    // Print first 5 elements
    printf("A[i] + B[i] = C[i]\n");
    for (int i = 0; i < 5; i++) {
        printf("%4.1f + %4.1f = %4.1f\n", A[i], B[i], C[i]);
    }

    printf("\nParallel Addition took %f seconds\n", duration);

    free(A);
    free(B);
    free(C);

    return 0;
}
