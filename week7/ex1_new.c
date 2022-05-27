#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

double taskA() {
    // Trivial to parallelize -> index i is private for each thread => no race condition
    #pragma omp for
    for (i=0; i<(int)sqrt(x); i++) {
        a[i] = 2.3*x;
        if (i < 10) b[i] = a[i];
    }
}

double taskB() {
    /* Not possible to parallelize.
    If flag is made private, other threads won't stop if one of the threads returns flag 1,
    in which case a gets updated elements which in fact should not be updated.
    If flag is shared, the order of execution of threads will affect the result,
    since the first thread that updates flag to 1 will break all other threads (=> requires threads to operate in correct order)
    */
    flag = 0;
    for (i = 0; (i<n) & (!flag); i++) {
        a[i] = 2.3 * i;
        if (a[i] < b[i]) flag = 1;
    }
}

double taskC() {
    /* Assuming foo does NOT depend on a[k] for k != i,
    the loop iterations are independent and trivial to parallelize.
    */
   #pragma omp parallel for
    for (i = 0; i < n; i++)
        a[i] = foo(i);
}

double taskD() {
    // Tasks are still independent -> trivial to parallelize
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        a[i] = foo(i);
        if (a[i] < b[i]) a[i] = b[i];
    }
}

double taskE() {
    // In this case the if-statement breaks out of entire loop,
    // so the point at which we break out of loop depends on order of execution of threads.
    // => Some threads may update a[i] with foo for an index appearing AFTER the loop should have been cut -> wrong results
    for (i = 0; i < n; i++) {
        a[i] = foo(i);
        if (a[i] < b[i]) break;
    }
}

double taskF() {
    dotp = 0;
    // dotp made private by reduction, which also protects it from race condition when summing up.
    #pragma omp parallel for reduction(+:dotp)
    for (i = 0; i < n; i++)
        dotp += a[i] * b[i];
}

double taskG() {
    #pragma omp parallel for
    for (i = k; i < 2*k; i++)
        a[i] = a[i] + a[i-k];
}

double taskH() {
    /*

    */
    for (i = k; i < n; i++)
        a[i] = b * a[i-k];
}