#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define CLOCKS_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

int *factorize() {
    
}

double fast_pow(double x) {
    // Since y is fixed, it is independent of any loops => optimize by loop invariance
    x = x * x; // x0 * x0 = x1
    x = x * x; // x1 * x1 = x2
    x = x * x * x * x * x; // x2 * x2 * x2 * x2 * x2 = x3
    x = x * x * x * x * x; // x3 * x3 * x3 * x3 * x3 = x4
    return x;
}

double fast_pow2(double x) {
    // Computations are fastest for operations for numbers in the 2-number system
    // Here: 100 can be split into 64+32+4 -> x^{100} = x^{64+32+4}
    double x4, x16, x32, x64;
    x4 = x * x * x * x;
    x16 = x4 * x4 * x4 * x4;
    x32 = x16 * x16;
    x64 = x32 * x32;
    return x64*x32*x4;
}

int main() {
    double x = 1.1;
    int y = 100;
    clock_t t_slow_start, t_slow_stop, t_fast1_start, t_fast1_stop, t_fast2_start, t_fast2_stop;

    t_slow_start = clock();
    double pow_func;
    for (int i=0; i<100000; i++){
        pow_func = pow(x, y);
    } 
    t_slow_stop = clock() - t_slow_start;
    clock_t t_slow_ms = CLOCKS_TO_MILLISEC(t_slow_stop);

    t_fast1_start = clock();
    double pow_own1;
    for (int i=0; i<100000; i++) {
        pow_own1 = fast_pow(x);
    }
    t_fast1_stop = clock() - t_fast1_start;
    clock_t t_fast1_ms = CLOCKS_TO_MILLISEC(t_fast1_stop);

    t_fast2_start = clock();
    double pow_own2;
    for (int i=0; i<100000; i++) {
        pow_own2 = fast_pow2(x);
    }
    t_fast2_stop = clock() - t_fast2_start;
    clock_t t_fast2_ms = CLOCKS_TO_MILLISEC(t_fast2_stop);

    printf("Time library function: %lu\n", t_slow_ms);
    printf("Result library function: %lf\n", pow_func);
    printf("Time own function 1: %lu\n", t_fast1_ms);
    printf("Result own function 1: %lf\n", pow_own1);
    printf("Time own function 2: %lu\n", t_fast2_ms);
    printf("Result own function 2: %lf\n", pow_own2);
}




