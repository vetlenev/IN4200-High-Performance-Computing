#ifndef WEEK5_H
#define WEEK5_H

#include <stdio.h>
#include <stdlib.h>

double *malloc1D_nopoint(double *b, int n) {
    b = (double*)malloc(n*sizeof(double));
    return b;
}

// Allocates array elements to memory globally, so no need to return
int malloc2D_point(double *(**A), int n, int m) {
    (*A) = (double**)malloc(n*sizeof(double*));
    (*A)[0] = (double*)malloc(n*m*sizeof(double));
    
    for (int i=1; i<n; i++) {
        (*A)[i] = &((*A)[0][i*m]);
    }

    return 0;
}

// Allocates array elements locally, so need to return to overwrite memory locations of A
double **malloc2D_nopoint(double **A, int n, int m) {
    A = (double**)malloc(n*sizeof(double*));
    A[0] = (double*)malloc(n*m*sizeof(double));

    for (int i=1; i<n; i++) {
        A[i] = &(A[0][i*m]);
    }

    return A;
}

int calloc2D_point(double *(**A), int n, int m) {
    (*A) = (double**)calloc(n, sizeof(double*));
    (*A)[0] = (double*)calloc(n*m, sizeof(double));
    
    for (int i=1; i<n; i++) {
        (*A)[i] = &((*A)[0][i*m]);
    }

    return 0;
}

double *calloc1D_nopoint(double *a, int n) {
    a = (double*)calloc(n, sizeof(double));
    return a;
}

double assignRand(double min, double max) {
    double scaled_rand = (double)(rand() / RAND_MAX);
    return min + scaled_rand*(max - min);
}

#endif