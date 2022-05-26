#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void gauss_elim(double **A, double *b, double *y) {
    for (int k=0; k<n; k++) {
        for (int j=k+1; j<n; j++)
            A[k,j] = A[k,j]/A[k,k];
        y[k] = b[k]/A[k,k];
        A[k,k] = 1;
        for (int i=k+1; i<n; i++) {
            for (int j=k+1; j<n; j++)
                A[i,j] = A[i,j] - A[i,k]*A[k,j];
            b[i] = b[i] - A[i,k]*y[k];
            A[i,k] = 0;
        }
    }
}

void gauss_elim_omp_new(double **A, double *b, double *y) {
    // Inner loop indices depends on outer loop index -> not a problem if both mase private,
    // BUT since we are updating array elements based on outer loop index,
    // race condition is likely to occur depending on the exact order the threads perform calculations.
    int i, j, k;
    for (k=0; k<n; k++) {
        // Outer loop index serialized.
        // Parallel region initiated n times => overhead!
        #pragma omp parallel
        {
        #pragma omp for // trivial to parallelize when k is shared among threads
        for (j=k+1; j<n; j++)
            A[k,j] = A[k,j]/A[k,k];
        #pragma omp single
        {
            y[k] = b[k]/A[k,k];
            A[k,k] = 1;
        }
        #pragma omp for private(j)
        for (i=k+1; i<n; i++) {
            for (j=k+1; j<n; j++)
                A[i,j] = A[i,j] - A[i,k]*A[k,j]; // since neither i nor j is k, A[i,k] nor A[k,j] will get updated in this nested for-loop
            b[i] = b[i] - A[i,k]*y[k];
            A[i,k] = 0;
        }
        }
    }
}






void gauss_elim_omp(double **A, double *b, double *y) {
    int i, j, k;
    #pragma omp parallel
    {
    // Inner loop index (*) depends on outer loop index,
    // so there is potential race condition is the two sets of indices are not assigned consistently
    for (k=0; k<n; k++) {
        #pragma omp for
        for (j=k+1; j<n; j++) // *
            A[k,j] = A[k,j]/A[k,k];
        #pragma omp single // Outer loop is done sequentially by threads anyway, so using single saves computing time
        {
        y[k] = b[k]/A[k,k];
        A[k,k] = 1;
        }
        // Inner loop index here does not depend on outer loop index -> can parallelize outer loop
        #pragma omp for private(j)
        for (i=k+1; i<n; i++) {
            for (j=k+1; j<n; j++)
                A[i,j] = A[i,j] - A[i,k]*A[k,j];
            b[i] = b[i] - A[i,k]*y[k];
            A[i,k] = 0;
        }
    }
    }
    // COMMENTS:
    /* 
    - Since inner loops depend on outer loop index, the loop trip count may vary
    much between threads, causing severe LOAD IMBALANCE. Different schedule could help.
    - Because we can't simply parallelize outermost loop, there are many omp-for initializations,
    i.e. spawning and synchronization of threads which yields OVERHEAD.
    - Last nested loop is well parallelized (due to independence) and gives little load imbalance and overhead.
    */
}

int main() {
    n = 200;
    double **A = malloc(n*sizeof *A);
    A[0] = malloc(n*n*sizeof A);
    double *b = malloc(n*sizeof b);
    double *y = malloc(n*sizeof y);

    for (int i=1; i<n; i++) {
        A[i] = &A[0][i*n];
    }

    for (int i=0; i<n; i++) {
        b[i] = (n-i)%4;
        for (int j=0; j<n; j++) {
            A[i][j] = i%2 + j*0.01;
        }
    }

    gauss_elim(A, b, y);

    return 0;
}