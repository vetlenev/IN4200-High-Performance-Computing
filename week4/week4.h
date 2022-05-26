#ifndef WEEK4_H // "include guard" - only defined the content of this header file if it's not already declared in program => prevents double/redundant assignments
#define WEEK4_H

#include <stdlib.h> // rand, malloc and free.
#include <stdio.h>  // printf

double randfrom(double min, double max)
{
    // return random value in range (min, max)
    //return min + (rand() / (double)RAND_MAX) * (max - min);
    return 5.0;
}

int malloc2D(double *(**A), int n, int m) 
// A: address of 2D matrix,  n: rows, m: cols
// Since we extract address from 2D array, we add * in front of the 2D-array argument. The paranthesis is provided to clarify it is a pointer to a 2D array
{
    (*A) = (double**)malloc(n*sizeof(double*)); // Dereferencing inside function changes value of global param. This line dereferences the whole 2D array
    (*A)[0] = (double*)malloc(n*m*sizeof(double)); // Dereferncing first row -> contiguous memory allocation

    for (int i=1; i<n; i++) {
       (*A)[i] = &((*A)[0][i*m]); // Same here -> must dereference to allocate memory of the actual matrix supplied
    }

    return 0;
}

int assignRand(double *(**A), int n, int m, double min, double max) {
    for (int i=0; i<n; i++) {
        for (int j=0; j<m; j++) {
            (*A)[i][j] = randfrom(min, max);
        }
    }
    return 0;
}

double **matrix_multiply(double **A, double **B, int n, int m, int p) {
    // A and B are NOT taken in as pointers, because we don't want to change them, just use them to construct C
    double **C1;
    malloc2D(&C1, n, p); // Important to pass address of C so the pointer argument allows dereferencing

    for (int i=0; i<n; i++) {
        for (int j=0; j<p; j++) {
            C1[i][j] = 0.0; // could have used calloc instead but then we can't use malloc2D
        }
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<p; j++) {
            for (int k=0; k<m; k++) {
                C1[i][j] = C1[i][j] + A[i][k]*B[k][j]; // Since tmp does not depend on k, it goes to register and it does not count as a load or store
            }
        }
    }

    // LOADS + STORES:
    // 2*n*p*m loads (for A and B) for innermost loop
    // 2*n*p loads/stores (one load and one store for C) for second innermost loop
    // Data traffic: 2*n*p*m + 2*n*p = 2*n*p*(m + 1)

    return C1;
}

double **matrix_multiply_unroll(double **A, double **B, int n, int m, int p) {
    /* Fast matrix multiplication including loop unrolling.
    Run through outermost loop with a stride of M. The next inner loop is then repeated m times - to ensure accounting for all elements in the stride.

    */
    double **C2;
    malloc2D(&C2, n, p); // Important to pass address of C so the pointer argument allows dereferencing

    for (int i=0; i<n; i++) {
        for (int j=0; j<p; j++) {
            C2[i][j] = 0.0; // could have used calloc instead but then we can't use malloc2D
        }
    }

    int N = 4; // 4-way unroll of outermost loop, gives a 2-fold reuse of B (2 for each outer loop iteration)
    int P = 4; // 4-way unroll of second outermost loop: 2-fold reuse of A (2 for each second outer loop)

    // Unroll and jam: combine outer loop unrolling with inner loop fusion
    for (int i=0; i<n; i++) { // Using stride on i, B[k][j] is loaded fewer times than necessary
        for (int j=0; j<p-(p%P); j+=P) {
            for (int k=0; k<m; k++) {
                C2[i][j] = C2[i][j] + A[i][k]*B[k][j]; // Since C[i][j] does not depend on k, it goes to register and it does not count as a load or store
                C2[i][j+1] = C2[i][j+1] + A[i][k]*B[k][j+1];
                C2[i][j+2] = C2[i][j+2] + A[i][k]*B[k][j+2];
                C2[i][j+3] = C2[i][j+3] + A[i][k]*B[k][j+3];
            }
        }
        // Remainder loop:
        for (int rj=p-(p%P); rj<p; rj++) { // Increments for remainder loop must be done with stride 1, so to cover all columns
            for (int k=0; k<m; k++) {
                C2[i][rj] = C2[i][rj] + A[i][k]*B[k][rj]; // Since C[i][j] does not depend on k, it goes to register and it does not count as a load or store
            }
        }
    }


    // LOADS + STORES (using M-stride traversal only on outer loop):
    // - n*p*m loads for A in innermost loop
    // - n*p*m/M loads for B in innermost loop
    // - 2*n*p load/store for C in secondmost inner loop
    // Data traffic: n*p*m + n*p*m/M + 2*n*p = 2*n*p*(m/2 + m/(2M) + 1)
    // 2*n*p*(m/2 + m/(2M) + 1) < 2*n*p*(m + 1) for M > 1

    return C2;
}

double **matrix_multiply_tmp(double **A, double **B, int n, int p, int m) {
    double **C3;
    malloc2D(&C3, n, p);

    for (int i=0; i<n; i++) {
        for (int j=0; j<p; j++) {
            C3[i][j] = 0.0; // could have used calloc instead but then we can't use malloc2D
        }
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<p; j++) {
            double tmp = C3[i][j]; // For large inner loop j, tmp prevents having to access different elements of cache line, putting less pressure on register
            for (int k=0; k<m; k++) {
                tmp = tmp + A[i][k]*B[k][j]; // Since tmp does not depend on k, it goes to register and it does not count as a load or store
            }
            C3[i][j] = tmp;
        }
    }

    return C3;
    
}   

#endif
