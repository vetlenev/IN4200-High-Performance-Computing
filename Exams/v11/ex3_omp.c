#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double trapezoidal (int n) {
    double result = 0.0;
    double h = 1.0/n;
    double x;
    int i;

    x = 0.0;
    for (i=1; i<n; i++) {
        x += h;
        result += exp(5.0*x)+sin(x)-x*x;
    }

    printf("Result: %lf\n", result);
    x = 0.;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    x = 1.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    return (h*result);
}

double trapezoidal_omp (int n) {
    double result = 0.0;
    double h = 1.0/n;
    double x = 0.0;
    int i;

    #pragma omp parallel
    {
    #pragma omp for reduction(+:result,x) // result and x automatically made firstprivate
    for (i=1; i<n; i++) {
        x = i*h; // No reduction operation => no race condition -> each x private/unique for each thread
        result += exp(5.0*x)+sin(x)-x*x;
    }

    printf("result: %lf\n", result);
    #pragma omp master // no race condition at this point, since all other threads are done updating result (because of implicit synchronization at end)
    {
    x = 0.;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    x = 1.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);
    }
    }
    
    return (h*result);
}

int main() {
    int n = 1000;
    double serial_result = trapezoidal(n);
    double parallel_result = trapezoidal_omp(n);

    printf("Serial: %lf\n", serial_result);
    printf("Parallel: %lf\n", parallel_result);

    return 0;
}