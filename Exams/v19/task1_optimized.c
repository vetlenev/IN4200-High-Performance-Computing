#include <stdio.h>
#include <stdlib.h>

void unoptimized() {
    int n = 10000;
    int **a = malloc(n*sizeof a*);
    a[0] = malloc(n*n*sizeof a);
    int **b = malloc(n*n*sizeof b*);
    b[0] = malloc(n*n*sizeof b);
    double *c = malloc(n*sizeof c);
    int *d = malloc(n*sizeof d);
    int *e = malloc(n*sizeof e);

    for (int i=1; i<n; i++) {
        a[i] = &a[0][i*n];
        b[i] = &b[0][i*n];
    }

    double *sin_part = malloc(n/2*sizeof sin_part);
    double exp_term = exp(1.0/n);
    double exp_accum = 1; // exp(0/n) = 1

    for (int i=0; i<n; i+=4) {
        if i > 0
            exp_accum = exp_accum * exp_term;
        if i<n/2 {
            sin_part[i] = sin(pi*i/n);
            c[i] = exp_accum + sin_part[i];
        }
        else {
            c[i] = exp_accum + (-sin_part[i-n/2])
        }
        for (int j=0; j<n; j++) {
            a[i][j] = b[i][j] + d[i]*e[j];
            a[i+1][j] = b[i+1][j] + d[i+1]*e[j];
            a[i+2][j] = b[i+2][j] + d[i+2]*e[j];
            a[i+3][j] = b[i+3][j] + d[i+3]*e[j];
        }
    }

}