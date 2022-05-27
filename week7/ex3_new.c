#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

double *dense_mat_vec(int m, int n, double *x, double *A, double *y)
{
    int i, j;
    for (i=0; i<m; i++)
    {
        double tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j]*y[j];
        x[i] = tmp;
    }

    return x;
}

double *dense_mat_vec_omp(int m, int n, double *x, double *A, double *y)
{
    int i, j;
    double tmp;
    #pragma omp parallel for private(j) reduction(+:tmp)
    for (i=0; i<m; i++)
    {
        tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j]*y[j];
        x[i] = tmp;
    }

    return x;
}

int main() {
    int m=10, n=5;
    double *A = malloc(m*n*sizeof *A);
    double *x1 = malloc(m*sizeof x1);
    double *x2 = malloc(m*sizeof x2);
    double *y = malloc(n*sizeof y);

    for (int i=0; i<n; i++) {
        y[i] = i*(i%3);
    }
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            A[i*n+j] = i%2 + (j%4)*i;
        }
    }

    double *xs = dense_mat_vec(m, n, x1, A, y);
    double *xp = dense_mat_vec_omp(m, n, x2, A, y);

    printf("xs[4] = %lf\n", xs[4]);
    printf("xp[4] = %lf\n", xp[4]);

    for (int i=0; i<m; i++) {
        if (xs[i] != xp[i]) {
            printf("Element i=%d wrong. %lf != %lf\n", i, xs[i], xp[i]);
            return 1;
        }
    }

    return 0;
}
