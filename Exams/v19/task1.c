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

    for (int i=0; i<n; i++) {
        c[i] = exp(1.0*i/n) + sin(pi*i/n);
        for (int j=0; j<n; j++) {
            a[j][i] = b[j][i] + d[j]*e[i];
        }
    }

}