#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define CONSTANT 10.0

void process_array(long n) {
    double *A = (double *)malloc(n * sizeof(double));
    if (A == NULL) return;

    // Initialize
    for (long i = 0; i < n; i++) {
        A[i] = (double)i;
    }

    double start = omp_get_wtime();

    // Parallel constant addition
    #pragma omp parallel for schedule(static)
    for (long i = 0; i < n; i++) {
        A[i] += CONSTANT;
    }

    double end = omp_get_wtime();

    printf("Size:\t%ld\n", n);
    printf("Time:\t%f seconds\n", end - start);

    long indices[] = {0, n/2, n-1};
    printf("Index\tOriginal\tConstant\tResult\n");

    for (int k = 0; k < 3; k++) {
        long idx = indices[k];
        printf("%ld\t%.1f\t\t%.1f\t\t%.1f\n",
               idx, (double)idx, CONSTANT, A[idx]);
    }

    printf("\n");

    free(A);
}

int main() {
    long sizes[] = {10000, 1000000, 10000000};

    for (int i = 0; i < 3; i++) {
        process_array(sizes[i]);
    }

    return 0;
}
