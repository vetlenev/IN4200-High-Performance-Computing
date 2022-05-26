#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double *mat_vec(int n, double **A, double *x, double *y) {
    for (int i=0; i<n; i++) {
        double tmp = 0; // temporary variable to hold a particular entry of y. Resides in register
        for (int j=0; j<n; j++) {
            tmp += A[i][j]*x[j];
        }
        y[i] = tmp; // one store
    }
    return y;
}

double *mat_vec_omp(int n, double **A, double *x, double *y) {
    double tmp;
    int i, j;
    #pragma omp parallel for private(j) reduction(+:tmp)
    for (i=0; i<n; i++) {
        tmp = 0; // temporary variable to hold a particular entry of y. Resides in register
        for (j=0; j<n; j++) {
            tmp += A[i][j]*x[j];
        }
        y[i] = tmp; // one store
    }
    return y;
}

int main() {
    int n=1000;

    double **A = malloc(n*sizeof *A);
    A[0] = malloc(n*n*sizeof A);
    for (int i=1; i<n; i++) {
        A[i] = &A[0][i*n];
    }
    double *x = malloc(n*sizeof x);
    double *y_s = malloc(n*sizeof y_s);
    double *y_p = malloc(n*sizeof y_p);
    for (int i=0; i<n; i++) {
        x[i] = i*0.01;
        for (int j=0; j<n; j++) {
            A[i][j] = i%5 + j%2; // Shortest direct graph
        }
    }

    y_s = mat_vec(n, A, x, y_s);
    y_p = mat_vec_omp(n, A, x, y_p);

    for (int i=0; i<n; i++) {
        if (y_s[i] != y_p[i]) {
            printf("FAIL on element (%d). %lf != %lf\n", i, y_s[i], y_p[i]);
            return 1;
        }
    }

    return 0;
}