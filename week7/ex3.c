#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "matvec.h"

int main() {
    int m=20000, n=10000;
    int i, j;
    double **A = (double**)malloc(m*sizeof(double*));
    A[0] = (double*)malloc(m*n*sizeof(double));
    double *x = (double*)malloc(m*sizeof(double));
    double *y = (double*)malloc(n*sizeof(double));

    #pragma omp parallel for
    for (i=1; i<m; i++) {
        A[i] = &(A[0][i*n]);
    }

    #pragma omp parallel for private(j)
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) {
            A[i][j] = 0.001*i + 0.002*j;
        }
    }
    #pragma omp parallel for
    for (j=0; j<n; j++) {
        y[j] = (n-j)*0.001;
    }

    double time0 = omp_get_wtime();
    dense_mat_vec(m, n, x, A, y); // A* : send in contiguous array, not matrix A
    double time1 = omp_get_wtime();
    double tot_time = time1 - time0;
    printf("Execution time no omp: %lf\n", tot_time);

    double time0_omp = omp_get_wtime();
    dense_mat_vec_omp(m, n, x, A, y); // Passing arguments as arrays won't change the global values => no need to reassign values
    double time1_omp = omp_get_wtime();
    double tot_time_omp = time1_omp - time0_omp;
    printf("Execution time omp: %lf\n", tot_time_omp);


    free(A[0]);
    free(A);
    free(x);
    free(y);

}