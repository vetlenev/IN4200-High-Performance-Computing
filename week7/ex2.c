#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

int serial(int n) {
    double dotp_s = 0;
    int i;
    double *as = (double*)malloc(n*sizeof(double));
    double *bs = (double*)malloc(n*sizeof(double));

    clock_t ts0 = clock();
    for (i=0; i<n; i++) {
        dotp_s += as[i]*bs[i];
    }
    clock_t ts1 = clock();
    double tot_s = (double) (ts1 - ts0)/CLOCKS_PER_SEC;
    printf("Serial execution: %lf\n", tot_s);

    return 0;
}

int parallel(int n) {
    double dotp_p = 0;
    int i;
    double *ap = (double*)malloc(n*sizeof(double));
    double *bp = (double*)malloc(n*sizeof(double));

    clock_t tp0 = clock();
    #pragma omp parallel for reduction(+:dotp_p) schedule(static, 1024)
    for (i=0; i<n; i++) {
        dotp_p += ap[i]*bp[i];
    }
    clock_t tp1 = clock();
    double tot_p = (double) (tp1 - tp0)/CLOCKS_PER_SEC;
    printf("Parallel execution: %lf\n", tot_p);

    return 0;
}

int main() {
    int n = 3e8;
    //serial(n);
    parallel(n);
}