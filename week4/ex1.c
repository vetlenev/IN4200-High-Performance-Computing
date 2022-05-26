// MEASURING MEMORY BANDWIDTH   

/*
Theoretical bandwidth is given by peak performance, which gives the maximum bandwidth possible
of the cpu. Normally, this is unachievable since there will always be some kind of latency,
cache misses or stalled instructions. 
To measure the realistically achievable bandwidth, we use a for-loop to measure the time
it takes to perform a loop iteration for a given number of loads, stores and flops. 
Averaging the time over all iterations yields a more accurate estimate.

1. Calculate code balance Bc
2. Find machine balance Bm
3. Calculate expected maximum fraction of peak performance l = min(1, Bm/Bc)
4. Calculate actual performance P = l*Pmax (where Pmax - peak performance - must be found)
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "week4.h"

#define CLOCK_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

int main() {

    int N = 10000000; // Make data set large enough to ensure performance is really memory-bounded
    double s = 1.1;
    int stride = 5;
    double *A = malloc(N * sizeof *A);
    double *B = malloc(N * sizeof *B);

    // TASK 1
    srand(time(NULL)); // Set random seed
    for (size_t i = 0; i < N; i++) {
        A[i] = sin(i);
        B[i] = cos(i);
    }

    clock_t start1 = clock();
    for (int i=0; i<N; i++) {
        A[i] = s*B[i]; // Will yield good approx to peak performance since computations are vectorizable
    }
    clock_t end1 = clock();
    double tot_time1 = CLOCK_TO_MILLISEC((double)(end1 - start1));
    //double comparison_time1 = (double)(1.0/stride) * tot_time1;

    int nr_bytes1 = N * (sizeof *A + sizeof *B); // total bytes for storing and loading (s is constant scalar - assumed to be in cache all time => no load)
    int flops1 = N/(tot_time1*1000); // floating-point operations per sec

    // TASK 2
    // Reload array values
    for (size_t i = 0; i < N; i++) {
        A[i] = sin(i);
        B[i] = cos(i);
    }

    clock_t start2 = clock();
    for (int i=0; i<N; i+=stride) {
        A[i] = s*B[i]; // Will yield good approx to peak performance since computations are vectorizable
    }
    clock_t end2 = clock();
    double tot_time2 = CLOCK_TO_MILLISEC((double)(end2 - start2));
    //double comparison_time2 = tot_time2; // 1/(N/stride) = stride/N
    int N2 = N/stride; // calculate nr iterations once, to avoid having to do the division multiple times
    
    int nr_bytes2 = N/stride * (sizeof *A + sizeof *B); // total bytes for storing and loading
    int flops2 = N2/(tot_time2*1000);

    printf("Nr bytes stride-1: %d\n", nr_bytes1);
    printf("Total time stride-1: %lfms\n", tot_time1);
    printf("FLOPS stride-1: %d\n", flops1);
    //printf("Avg time stride-1: %u\n", comparison_time1);
    printf("Nr bytes stride-%d: %d\n", stride, nr_bytes2);
    printf("Total time stride-%d: %lfms\n", stride, tot_time2);
    printf("FLOPS stride-%d: %d\n", stride, flops2);
    //printf("Avg time stride-%d: %u\n", stride, comparison_time2);

    free(A);
    free(B);
    // No need to free elements separately, since freeing an n-D array will free its (n-1)-D elements
}