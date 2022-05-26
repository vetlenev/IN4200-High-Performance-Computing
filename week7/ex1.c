#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

// TASK A
double taskA() {
    /* PARALLELIZABLE
    - Each loop iteration computation independent of each other - parallelizable
    - x is static variable => should be shared among threads (not privatized)
    - sqrt is heavy operation and should only be calculated by one thread
    - if-test differs for each iteration - no parallelization potential
    */
    int i;
    double x = 5e16;
    double S = sqrt(x);
    double *a = (double*)malloc((int)S*sizeof(double));
    double *b = (double*)malloc((int)S*sizeof(double));

    // SERIAL
    clock_t ts0 = clock();
    for (i=0; i<(int)S; i++) { // i is automatically privatized by openmp
        a[i] = 2.3 * x;
        if (i < 10) b[i] = a[i];
    }
    clock_t ts1 = clock();
    double tot_s = (double) (ts1 - ts0)/CLOCKS_PER_SEC;
    printf("Serial execution: %lf\n", tot_s);

    //free(a);
    //free(b);

    // PARALLEL
    //double *a = (double*)malloc((int)S*sizeof(double));
    //double *b = (double*)malloc((int)S*sizeof(double));

    clock_t tp0 = clock();
    #pragma omp parallel for // no private variables needed -- all are static
    for (i=0; i<(int)S; i++) { // i is automatically privatized by openmp
        a[i] = 2.3 * x;
        if (i < 10) b[i] = a[i];
    }
    clock_t tp1 = clock();
    double tot_p = (double) (tp1 - tp0)/CLOCKS_PER_SEC;
    printf("Parallel execution: %lf\n", tot_p);

    return tot_p;
}

double taskB() {
    /* NOT PARALLELIZABLE
    - i is automatically privatized by openmp
    - each loop iteration computation independent
    - Flag dynamically changed in loop => if parallelized, other threads may continue assigning values to a[i] if they are ahead of the thread that hits the flag.
    But barrier (synchronization of threads) after if-statement won't help, since we have no guarantee that the thread that hits the flag was the last one to be executed,
    which means that a-values AHEAD of the flag-hit-index may have (wrongly) been assigned values by other threads.
    */
    int i, n=1000000, flag=0;
    double *a = (double*)malloc(n*sizeof(double));
    double *b = (double*)malloc(n*sizeof(double));

    for (i=0; i<n; i++) {
        b[i] = 0.00001*i*i;
    }

    // SERIAL
    clock_t ts0 = clock();
    for (i=0; (i<n) & (!flag); i++) {
        a[i] = 2.3 * i;
        if (a[i] < b[i]) flag = 1;
    }
    printf("Stop idx: %d\n", i);
    clock_t ts1 = clock();
    double tot_s = (double) (ts1 - ts0)/CLOCKS_PER_SEC;
    printf("Serial execution: %lf\n", tot_s);

    // PARALLEL
    flag=0;
    clock_t tp0 = clock();
    #pragma omp parallel for firstprivate(flag)
    for (i=0; (i<n) & (!flag); i++) { // NB: this won't compile because definition of loop-iteration does not fulfill requirements for openmp parallelization
        a[i] = 2.3 * i;
        if (a[i] < b[i]) flag = 1;
        #pragma omp barrier // avoid threads proceeding to next iteration before all elements checked
    }
    printf("Stop idx: %d\n", i);
    clock_t tp1 = clock();
    double tot_p = (double) (tp1 - tp0)/CLOCKS_PER_SEC;
    printf("Parallel execution: %lf\n", tot_p);

    return tot_p;
}

double taskC() {
    /* PARALLELIZABLE
    - Parallelization depends on code inside function foo. 
    - If foo depends on a[i], for other iterations (i+1, i-1, ...), we have a loop dependency
    and loop is not straightforward parallelizable.
    - If foo does not depend on a[:] (or only a[i]) loop is parallelizable, but additional
    parallel-directives (barrier, critical, ...) may be needed depending on the content of foo, 
    e.g. if something needs to be done one thread at a time.
    */ 
    int i, n;
    double *a = (double*)malloc(n*sizeof(double));
    double foo(double j) {
        return j;
    }

    #pragma omp parallel for 
    for (i = 0; i < n; i++) {
        a[i] = foo(i);
    }
}

double taskD() {
    /* PARALLELIZABLE
    - Loop satisfies "Canonical Loop Form" for openmp
    - Each loop iteration independent (assuming foo not function of a[j], j != i)
    - Each if statement independent - no requirement of synchronization or fixed ordering
    */
    int i, n;
    double *a = (double*)malloc(n*sizeof(double));
    double *b = (double*)malloc(n*sizeof(double));
    double foo(double j) {
        return j;
    }

    for (i=0; i<n; i++) {
        a[i] = foo(i);
        if (a[i] < b[i]) a[i] = b[i];
    }
}

double taskE() {
    /* NOT PARALLELIZABLE
    - Canonical loop form satisfied
    - First line independent
    - Elements assigned to a[i] after first break-occurence will differ for serial vs parallel
    depending on the order the threads iterate through a[i] --> CAN give wrong results.
    */
   for (i=0; i<n; i++) {
       a[i] = foo(i);
       if (a[i] < b[i]) break;
   }
}

double taskF() {
    /* PARALLELIZABLE
    - Canonical loop form satisfied
    - Individual dot product computations independent
    - Assignment to shared variable must be properly scheduled by OpenMP:
    Reduction clause with sum-operation required for dotp, to avoid race condition between threads.
    */
    dotp = 0;
    for (i=0; i<n; i++) {
        dotp += a[i] * b[i];
    }
}

double taskG() {
    /* PARALLELIZABLE - NOT STRAIGHFORWAR
    - Loop dependency - no straightforward parallelization
    - Canonical loop form satisfied
    - Because the dependency has a jump of k, every k'th set of iterations are independent,
    and k threads can work in parallel for k successive iterations
    - No race condition, because i-k is (for every iteration) outside the scope of updates
    because we only loop from k to 2*k. So it doesn't matter what order the threads execute the code in - 
    their updating result won't influence the computations of the other threads.
    k = 4:
    - a[4], a[5], a[6] and a[7] can be worked on be 4 different threads
    */
    for (i=k; i < 2*k; i++) {
        a[i] = a[i] + a[i-k];
    }
}

double taskH() {
    /* PARALLELIZABLE - NOT STRAIGHTFORWARD 
    - Canonical loop form satisfied
    - b: static scalar - shared by all threads without having to be privatized
    - Assuming n > 2*k there will be a race condition between threads. 
    The ordering of the updates matter: every k'th set of threads can work independently, but once complete they must be synchronized.
    - Assuming n < 2*k, no race condition.
    */
    for (i=k; i<n; i++) {
        a[i] = b * a[i-k];
    }
}

int main() {
    taskA();
    //taskB();

    return 0;
}