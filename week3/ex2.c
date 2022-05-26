/* DOT PRODUCT + PREFETCHING
Assumptions:
- CPU can do one load/store, one multiplication and one addition per clock cycle
- Latency of loop traversal and branching negligable
- Memory bus transfer: 3.2 GBytes/sec
- Load one cache line: 100 clock cycles
- Four doubles fit into one cache line
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N = 1000;
double s = 0.0;
double *A = (double*)calloc(N*sizeof(double));
double *B = (double*)calloc(N*sizeof(double));

for (int i=0; i<N; i++) {
    s = s + A[i]*B[i]
}

/*
for (int i=0; i<N; i++) {
    LOAD R1 = A[i], R2 = B[i]; // One load takes 110ns (10 for cache line itself, 100 for latency by the fourth element)
    // Two cache line loads => 220 ns
    // Two flops (MULT and ADD) for each entry in caches. Four entries loaded: 2*4 = 8 flops
    R1 = MULT(R1, R2) // can overwrite R1 after multiplying, since A[i] no longer needed
    R2 = ADD(s, R1)
    store s = R2
}
*/