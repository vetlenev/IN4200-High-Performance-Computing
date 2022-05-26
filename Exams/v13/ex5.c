#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define MIN(a, b) ((a<b)?a:b) 

void floyd_omp_new(double **A, int n) {
    double **D = malloc(n*sizeof *D);
    D[0] = malloc(n*n*sizeof D);
    for (int i=1; i<n; i++) {
        D[i] = &D[0][i*n];
    }
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            D[i][j] = MIN(A[i][j], A[j][i]); // Shortest direct graph
        }
    }

    int i, j, k;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            for (k=0; k<n; k++) {
                if (A[j][k] > A[j][i]+A[i][k]) // Direct distance longer than indirect distance
                    D[j][k] = A[j][i] + A[i][k];
            }
        }
    }
    /*
    Even though we assign to a new array D (getting rid of race condition for A),
    we still have the problem 
    */
}

int main() {
    return 0;
}