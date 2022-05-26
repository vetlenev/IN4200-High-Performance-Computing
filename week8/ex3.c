#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "week8.h"


int main() {
    // TASK A
    //test_sorting_algorithm();

    // TASK B+C
    //test_improved_sorting_algorithm();
    //a[0] = 1; a[1] = 2; a[2] = 4; a[3] = 3;
    //a[4] = 5; a[5] = 7; a[6] = 6; a[7] = 8;

    int n = 10000;
    float *a = (float*)malloc(n*sizeof(float));
    float *b = (float*)malloc(n*sizeof(float));
    float *copy = (float*)malloc(n*sizeof(float));
    srand((unsigned) 1); // set seed of rng
    for (int i=0; i<n; i++) {
        copy[i] = rand() % 1000;
    }
    for (int i=0; i<n; i++) {
        a[i] = copy[i];
        b[i] = copy[i];
    }

    // TASK D
    clock_t ts0 = clock();
    serial_odd_even_improved(n, &a);
    clock_t ts1 = clock();
    double tot_ts = (double)(ts1 - ts0) / CLOCKS_PER_SEC;
    printf("Serial execution time: %lf\n", tot_ts);

    double tp0 = omp_get_wtime();
    serial_odd_even_omp(n, b); // take in address of a (pointer), so its values can be changed in-place by the function
    double tp1 = omp_get_wtime();
    printf("Parallel execution time: %lf\n", tp1-tp0);
    printf("b[500]: %lf\n", b[500]);
    printf("b[501]: %lf\n", b[521]);
    printf("b[502]: %lf\n", b[542]);

    return 0;
}