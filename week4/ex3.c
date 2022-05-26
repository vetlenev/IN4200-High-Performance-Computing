// MATRIX-MATRIX MULTIPLICATION
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "week4.h"

#define CLOCK_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

int nr_loads_stores(int n, int m, int p) {
    // A ~ (nxm). B ~ (mxp)
    int stores = n*p; // += operator does not count as load or store since it is a temporary variable not dependent on innermost loop - so it resides in register. Store is only invoked when assignment to element (i,j) is done and we move on to next element.
    int loads_bigLc = 2*m*n*p; // Assuming one cache line can hold at least 2*m elements
    int flops = 2*n*m*p; // 2 because we must include += and *
}

int main() {
    int n = 1000, m = 1000, p = 1000;
    double min = 0, max = 5;
    double **A; // A ~ (n x m)
    double **B; // B ~ (m x p)
    malloc2D(&A, n, m); // when passing ADDRESS of a 2D array, the argument must be a pointer to a 2D array (so written like a triple pointer)
    malloc2D(&B, m, p);

    assignRand(&A, n, m, min, max);
    assignRand(&B, m, p, min, max);
    printf("A[3][2]: %lf\n", A[3][2]);

    clock_t start1 = clock();
    double **C1 = matrix_multiply(A, B, n, m, p);
    clock_t end1 = clock();

    clock_t start2 = clock();
    double **C2 = matrix_multiply_unroll(A, B, n, m, p);
    clock_t end2 = clock();

    clock_t start3 = clock();
    double **C3 = matrix_multiply_tmp(A, B, n, m, p);
    clock_t end3 = clock();

    double tot1 = CLOCK_TO_MILLISEC((double)(end1 - start1));
    double tot2 = CLOCK_TO_MILLISEC((double)(end2 - start2));
    double tot3 = CLOCK_TO_MILLISEC((double)(end3 - start3));


    printf("Total time matmult:     %lfms\n", tot1);
    printf("Total time matmult unroll: %lfms\n", tot2);
    printf("Total time matmult tmp: %lfms\n", tot3);
    printf("Speedup unroll: %.3lf\n", tot1/tot2);
    printf("Speedup tmp: %.3lf\n", tot1/tot3);


    return 0;
}