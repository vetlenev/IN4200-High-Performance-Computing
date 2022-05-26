#include <stdio.h>
#include <stdlib.h>

void dense_mat_vec(int m, int n, double *x, double *A, double *y)
{
    // Let m and n be local parts of full matrix A, and compute subparts serially
    int i, j;
    for (i=0; i<m; i++)
    {
        double tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j]*y[j];
        x[i] = tmp;
    }
}

void dense_mat_vec_omp(int m, int n, double *x, double *A, double *y)
{
    int i, j;
    #pragma omp parallel for private(j)
    for (i=0; i<m; i++)
    {
        double tmp = 0.; // private for each thread
        for (j=0; j<n; j++)
            tmp += A[i*n+j]*y[j]; // since tmp is private, each thread will compute its own sum and assign to value to its OWN part of vector x => no race condition
        x[i] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Need two command line arguments: m and n");
        return 1;
    }
    else {
        m = atoi(argv[1]);
        n = atoi(argv[2]);
    }

    double **A = malloc(m*sizeof A);
    A[0] = malloc(m*n*sizeof *A);
    double *x = malloc(m*sizeof x);
    double *y = malloc(n*sizeof y);

    for (int i=1; i<m; i++) {
        A[i] = &A[0][i*n];
    }

    #pragma omp parallel 
    {
        // Initialize values
        #pragma omp for private(j)
        for (int i=0; i<m; i++) {
            x[i] = m - i*0.1;
            for (int j=0; j<n; j++) {
                A[i][j] = i*0.01 + j*0.1;
            }
        }

        #pragma omp for
        for (int j=0; j<n; j++)
            y[j] = j*0.01 + 1;
    }
    // Do computations
    dense_mat_vec_omp(int m, int n, double *x, double *A, double *y);

    // Task c
    #pragma omp parallel
    {
        // Block decomposition of A, x and y, into s row blocks and t col blocks
        base_m = m / s;
        rem_m = m % s;
        base_n = n / t;
        rem_n = n % t;
        my_m = base_m + ((thread_id<rem_m)?1:0);

        dense_mat_vec(int my_m, int my_n, double *x, double *A, double *y);
    }
}