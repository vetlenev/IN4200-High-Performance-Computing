#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>

void task4a() {
    double **A = malloc(100*100*sizeof *A);
    int i,j;
    #pragma omp parallel for default(shared) schedule(static)
    for (i=0; i<100; i++) {
        for (j=0; j<100; j++) {
            A[i][j] = A[i][j] * A[i][j];
        }
    }
    /* SOLUTION:
    - There are no dependencies in the nested loop 
    (computations by one rank does not involve indices of other ranks),
    but inner loop index is not made private for each thread.
    It is shared, potentially leading to race condition (accessing same j).
    */
}

double task4b_serial() {
    int i, N = 100;
    double s = 0.;
    double *a = (double*)malloc (N*sizeof(double));
    double *b = (double*)malloc (N*sizeof(double));
    for (i=0; i<N; i++) {
        a[i] = 10.0+2*i;
        b[i] = 20.0+sin(0.1*i);
    }
    for (i=0; i<N-1; i++) {
        s += a[i];
        a[i+1] = cos(b[i]);
    }
    return s;
}
double task4b_omp() {
    int i, N = 100;
    double s = 10.0; // Change to first value of original a
    double *a = (double*)malloc (N*sizeof(double));
    double *b = (double*)malloc (N*sizeof(double));

    #pragma omp parallel default(shared)
    {
    #pragma omp for
    for (i=0; i<N; i++) {
        //a[i] = 10.0+2*i; // Not needed, it is overwritten immediately anyway
        b[i] = 20.0+sin(0.1*i);
    }
    #pragma omp for // Compute new a values in a separate for loop to avoid race condition
    for (i=1; i<N; i++) {
        a[i] = cos(b[i-1]);
    }

    #pragma omp for reduction(+:s)
    for (i=1; i<N-1; i++) {
        s += a[i]; // Because we read a AFTER writing/updating it, it is allowed to do calculations of a first in a separate loop
        // If write-after-read, the updates must be done in a separate loop AFTER this loop
    }
    }
    return s;
}

int main() {
    double ss = task4b_serial();
    double sp = task4b_omp();
    printf("Serial: %lf\n", ss);
    printf("Parallel: %lf\n", sp);
    return 0; 
}