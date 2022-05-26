// Solution to week 3 ex. 1, timing division.

#include <stdlib.h>
#include <math.h> // M_PI
#include <stdio.h>
#include <time.h>

// This is to allow compilation on different compilers.
// If you are using gcc <x86intrin.h> is the right header.
// 
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

double numerical_integration (double x_min, double x_max, int slices);

int main(int argc, char const *argv[]) { // Command line args are sent as pointers
    int n_div;
    if (argc > 1) {
        n_div = atoi(argv[1]);
    } else {
        n_div = 10000;
    }

    // Testing the integration for 10, 100 and 1000 slices.
    int n = 3;
    int* slices = malloc(n * sizeof *slices);
    slices[0] = 10; // Using malloc to define the pointers, its elements are retrieved and assigned in same way as normal array

    for (size_t i = 1; i < n; i++) { // use size_t if don't want to assign variables yourself
        slices[i] = slices[i-1]*10; // Not vectorizable -> real dependency!
    }

    // Check accuracy of result as function of nr slices
    double value, rel_e;
    for (size_t i = 0; i < n; i++) {
        value = numerical_integration(0, 1, slices[i]);
        // Time we measure is less than theoretical time since compiler is smart enough to execute pieces of code concurrently
        // since floating-point operations are independent of each other (basically simple pipelining).
        // Theoretical time limit of floating-point division does not take this optimization into account.
        // To get result comparable to theoretical limit, specifify -O0 to prevent compiler from doing this smart optimization.
        rel_e = value/M_PI - 1;
        printf("slices: %*d, integral: %lf, relative error: %lf\n", // %*d sets of * (print) spots for variable d
               n+1, slices[i], value, rel_e); 
    }

    // Counting cycles.
    unsigned long long start = __rdtsc(); // using unsigned int we get one more order as storage (bit) in the byte, since we don't need the bit that used to represent the sign of the number
    /* __rdtsc() 
    Returns cpu time that has passed since pc was turned on.
    If cpu runs 2 GHz/sec, 2 billion clock cycles have passed during a second.
    */
    value = numerical_integration(0, 1, n_div);
    unsigned long long end = __rdtsc();

    double avg = (double)(end - start)/n_div; // Since one division is executed in each loop, dividing by number of loops gives estimate of average time used by division operator
    // The more slices we use to compute PI, the more accurate becomes our average time estimate
    // If doing loop just a few times, a significant part of total runtime will go to fetch required
    // data from memory location (latency). Latency for iterations after the first is negligable since
    // we utilize spatial locality on the cache line.
    // Increasing nr iterations will make the latency of first iteration negligable compared to
    // the division performed in each iteration.

    printf("Average number of cycles: %lf\n", avg);
    free(slices);

    return 0;
}


double numerical_integration (double x_min, double x_max, int slices)
{
    double delta_x = (x_max-x_min)/slices;
    double x, sum = 0.0;

    for (int i=0; i<slices; i++)
    {
        x = x_min + (i+0.5)*delta_x;
        sum = sum + 4.0/(1.0+x*x);
    }
    return sum*delta_x;
}
