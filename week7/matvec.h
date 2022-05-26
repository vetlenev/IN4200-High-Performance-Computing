#ifndef MATVEC_H
#define MATVEC_H

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void dense_mat_vec(int m, int n, double *x, double *A, double *y) {
    int i, j;

    for (i=0; i<m; i++) {
        double tmp=0;
        for (j=0; j<n; j++) {
            tmp += A[i*n+j]*y[j];
        }
        x[i] = tmp;
    }
    printf("No omp A[4][5]: %lf\n", A[4*n+5]);
}

void dense_mat_vec_omp(int m, int n, double *x, double *A, double *y) {
    int i, j;

    // Since tmp is scalar, each summation of tmp is independent of the others => no need for reduction-clause (BUT PROGRAMS RUN FASTER WITH REDUCTION ????)
    // HAS REDUCTION AND PRIVATIZING tmp THE SAME EFFECT (SAME SPEEDUP) ?? -> DO BOTH PERFORM SAME SYNCHRONIZATION OF RESULTS AT END OF LOOP ??
    #pragma omp parallel for private(j)
    for (i=0; i<m; i++) {
        double tmp = 0; // Defining variables inside parallel region will make it private for each thread !
        for (j=0; j<n; j++) {
            tmp += A[i*n+j]*y[j]; // Each thread can assign values to tmp independently, because the order of a sum on a scalar does not matter
        }
        x[i] = tmp;
    }
    printf("Omp A[4][5]: %lf\n", A[4*n+5]);
}

#endif