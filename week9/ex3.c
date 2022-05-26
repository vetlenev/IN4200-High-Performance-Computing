#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>

void unoptimized() {
    /* 
    Optimization potential:
    - Initialization of a and b can be parallelized (iterations not dependent on each other)
    - Use only one parallel region
    - Reduction clause to sum up s
    - Since all values of a is overwritten in second loop (except first element), the initialization of a is redundant - only need to assign first element
    
    Not possible:
    - Create lookup table for sin and cos have no effect, since all sin/cos values are unique and need to be calculated anyway --> it goes over many periods, but same values are not hit each new period
    */
    int i, N = 1000000;
    double s = 0.0;

    double *a = (double*)malloc(N*sizeof(double));
    double *b = (double*)malloc(N*sizeof(double));

    // Initialization
    for (i=0; i<N; i++) {
        a[i] = 10.0 + 2*i;
        b[i] = 20.0 + sin(0.1*i);
    }

    for (i=0; i<N-1; i++) {
        // PROBLEM: Since assignment depends on another iteration, we risk adding wrong value a[i] to s
        s += a[i]; // Must make sure a[i] has been updated with a[i+1] from previous iteration
        a[i+1] = cos(b[i]); 
    }

    printf("Serial: a[3460]: %lf\n", a[3460]);
}


void optimized() {
    int i, N = 1000000;
    double s = 0.0;

    double *a = (double*)malloc(N*sizeof(double));
    double *b = (double*)malloc(N*sizeof(double));

    #pragma omp parallel
    {
        #pragma omp single
        {
            a[0] = 10.0;
        }
        // Automatic synchronization at this point

        #pragma omp for reduction(+:s)
        for (i=0; i<N-1; i++) {
            b[i] = 20.0 + sin(0.1*i); // Moving b to this loop does not compute its last element, but last element of b is redundant anyway
            a[i+1] = cos(b[i]); // a and b independent => parallelizable
            s += a[i]; // protect with reduction to allow threads to compute different elements a[i] concurrently without affecting sum
            // Does not matter if summation is before or after a[i+1]-expression, since they are independent of one another. Last a[i+1] should not be summed to s anyway
        }
    }
    printf("Parallel: a[3460]: %lf\n", a[3460]);
}

int main() {

    clock_t ts0 = clock();
    unoptimized();
    clock_t ts1 = clock();
    double tot_time_s = (double)(ts1 - ts0) / CLOCKS_PER_SEC;
    printf("Serial time: %lf\n", tot_time_s);

    double tp0 = omp_get_wtime();
    optimized();
    double tp1 = omp_get_wtime();
    double tot_time_p = tp1 - tp0;
    printf("Parallel time: %lf\n", tot_time_p);

    return 0;
}