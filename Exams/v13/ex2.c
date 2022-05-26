#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double **transpose_serial(int n) {
    double **A = malloc(n*sizeof *A);
    A[0] = malloc(n*n*sizeof A);

    for (int i=1; i<n; i++) {
        A[i] = &A[0][i*n];
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            A[i][j] = i*0.01 + j%5;
        }
    }

    for (int k=0; k<n; k++) {
        // Even though inner loop depends on outer loop index,
        // the computations don't overlap -> no race condition for parallelization.
        for (int j=0; j<k; j++) {
            A[k][j] = A[j][k];
        }
    }
    printf("Serial results:\n");
    printf("A[%d][%d] = %lf\n", 2, 5, A[2][5]);
    printf("A[%d][%d] = %lf\n", 7, 1, A[7][1]);
    printf("A[%d][%d] = %lf\n", 37, 14, A[37][14]);
    printf("A[%d][%d] = %lf\n", 14, 37, A[14][37]);

    return A;
}

double **transpose_omp(int n) {
    double **A = malloc(n*sizeof *A);
    A[0] = malloc(n*n*sizeof A);

    for (int i=1; i<n; i++) {
        A[i] = &A[0][i*n];
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            A[i][j] = i*0.01 + j%5;
        }
    }

    int j, k;
    // Both inner and outer loop indices are made private,
    // and since inner loop only depends on outer index k,
    // each thread will get unique row-col pairs.
    #pragma omp parallel for private(j)
    for (k=0; k<n; k++) {
        for (j=0; j<k; j++) {
            A[k][j] = A[j][k];
        }
    }
    /*
    Parallel code suffers from severe load imbalance, because number of computations
    increase with 1 each new iteration. Outer iteration 2k will have k more computations
    than outer iteration k.
    To alleviate load imbalance, we can set up a dynamic scheduler that frequently assigns
    new work to idling threads, deloading the big tasks from later processes.

    Overhead is minor, because we only initialize a parallel region and spawn threads once.
    Synchronization only happens once - at the end of outer loop.
    */
    printf("Parallel results:\n");
    printf("A[%d][%d] = %lf\n", 2, 5, A[2][5]);
    printf("A[%d][%d] = %lf\n", 7, 1, A[7][1]);
    printf("A[%d][%d] = %lf\n", 37, 14, A[37][14]);
    printf("A[%d][%d] = %lf\n", 14, 37, A[14][37]);

    return A;
}

int main() {
    int n = 100;

    double **A_s = transpose_serial(n);
    double **A_p = transpose_omp(n);

    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            if (A_s[i][j] != A_p[i][j]) {
                printf("A[%d][%d] not equal: %lf != %lf\n", i, j, A_s[i][j], A_p[i][j]);
                return 1;
            }
        }
    }

    return 0;
}