#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double Euclidean_norm (double* a, int n)
{
    double sum = 0.;
    int i;
    for (i=0; i<n; i++)
        sum += a[i]*a[i];

    return sqrt(sum);
}

void loop_blocking(N, M, b, m) {
    for (int i=0; i<N; i+=b) {
        for (int j=0; j<M; j+=b) {
            for (int ii=i; i<i+b; i+=4) {
                for (int jj=j; j<j+b; j++) {
                    C[ii] += A[ii][jj] + B[jj];
                    C[ii+1] += A[ii+1][jj] + B[jj];
                    C[ii+2] += A[ii+2][jj] + B[jj];
                    C[ii+3] += A[ii+3][jj] + B[jj];
                }
            }
        }
    }
}


int main() {

    double sum = 0.;
    int n = 900; int i;
    double *a = malloc(n*sizeof a);

    #pragma omp parallel
    {
    #pragma omp for
    for (i=0; i<n; i++) {
        a[i] = i%5 + 0.001*i;
    }
    #pragma omp for reduction(+:sum)
    for (i=0; i<n; i++)
        sum += a[i]*a[i];

    #pragma omp master // Calculation done, so no syncrhonization required
    {
        sum = sqrt(sum);
        printf("Sum: %lf\n", sum);
    }
    }

    double serial_norm = Euclidean_norm(a, n);
    printf("Euclidean norm: %lf\n", serial_norm);

    return 0;
}