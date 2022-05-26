#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void serial() {
    int i, N = 100000;
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
}

void parallel() {
    int i, N = 100000;
    double s = 0.;
    double *a = (double*)malloc (N*sizeof(double));
    double *b = (double*)malloc (N*sizeof(double));

    #pragma omp parallel
    {
    // All elements of a overwritten in second loop - no need initializing a
    #pragma omp for
    for (i=0; i<N; i++) {
        //a[i] = 10.0+2*i;
        b[i] = 20.0+sin(0.1*i);
    }
    #pragma omp single {
        s = a[0];
    }
    // NB: a[i] is assigned to a while a[i+1] is updated => loop dependency
    #pragma omp for
    for (i=0; i<N; i++) {
        a[i+1] = cos(b[i]);
    }

    #pragma omp for reduction(+:s)
    for (i=0; i<N-1; i++) {
        s += a[i]; // No contention now
    }
    }
}