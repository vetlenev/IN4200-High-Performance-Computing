#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "matvec.h"

int main() {
    int m=20000, n=10000;
    int i, j;
    //double **A = (double**)malloc(m*sizeof(double*));
    //A[0] = (double*)malloc(m*n*sizeof(double));
    double *A = (double*)malloc(m*n*sizeof(double));
    double *x = (double*)malloc(m*sizeof(double));
    double *y = (double*)malloc(n*sizeof(double));

    #pragma omp parallel for private(j)
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) {
            A[i*n+j] = 0.001*i + 0.002*j;
        }
    }
    #pragma omp parallel for
    for (j=0; j<n; j++) {
        y[j] = (n-j)*0.001;
    }

    double time0 = omp_get_wtime();
    #pragma omp parallel
    {
        int t = omp_get_num_threads();
        int t_id = omp_get_thread_num();

        int m_start = (t_id*m) / t;
        int m_end = ((t_id+1)*m) / t; // THIS DOES AUTOMATICALLY CALCULATE REMAINDER, IF ANY ??
        // Send address of x and A as arguments as this will give correct input type (pointer *). x[m_start] is a float, so won't comply with the required *-type input argument
        dense_mat_vec(m_end-m_start, n, &x[m_start], &A[m_start*n], y); // &A: multiply m_start by n to get correct location of matrix row m_start in contiguous array
        // Incurs latency (reduced memory bandwidth) because x is only written to by one thread a time,
        // which is required to avoid cache write conflicts. Threads must therefore refetch a cache line if it has been updated.
    }
    double time1 = omp_get_wtime();
    double tot_time = time1 - time0;
    printf("Execution time omp: %lf\n", tot_time);

    free(A);
    free(x);
    free(y);

}
