#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLOCK_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC
#define _USE_MATH_DEFINES
#include <math.h>

double numerical_integration(double x_min, double x_max, int nr_steps) {
    double dx = (x_max - x_min)/nr_steps;
    double x, result = 0.0;
    clock_t time_start = clock();
    for (int i=0; i<nr_steps; i++) {
        // Use midpoint rule to approximate integral
        x = x_min + (double)((i+0.5)*dx); // casting to double not strictly necessary -> compiler recasts automatically between int and double
        result = result + 4.0/(1.0+x*x);
    }
    clock_t time_stop = clock() - time_start;
    clock_t time_ms = CLOCK_TO_MILLISEC(time_stop);
    printf("Time for %d steps: %lu\n", nr_steps, time_ms);
    return result*dx; // multiply by dx outside loop => reduce computations
}

int main() {
    for (int step=1e5; step<1e6; step+=1e5) {
        double num_val = numerical_integration(0.0, 1.0, step);
        printf("Result %d steps: %lf\n", step, num_val);
    }
    printf("\nTrue PI: %lf\n", M_PI);

    /* Measure latency of floating point division:
    ALTERNATIVE 1:
    Assume divide is so much slower than other floating-point operations that time used in loop is dominated by divide.
    Nr clock cycles per iteration then equals divide throughput (nr divides performed during this time).
    Total time (over loop) then equals divide latency.
    If we know total nr clock cycles, we can divide by nr iterations to get clock cycles per iteration
    => this number equals divide latency.

    Assuming all data needed resides in registers (and not cache/main), we can neglect the additional
    cost of bandwidth and cache miss, thus can assume all latency involved comes entirely from 
    extra clock cycles used for division (which is assumed to hold ALL clock cycles).
    */ 

    return 0;
}