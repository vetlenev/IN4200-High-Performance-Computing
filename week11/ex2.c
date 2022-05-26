// Parallelizing trapezoidal rule
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

double trapezoidal(int n) { 
    double result = 0.0; 
    double h = 1.0/n; 
    double x; int i; 
    
    x = 0.0; 
    for (i=1; i<n; i++) { 
        x += h; 
        result += exp(5.0*x)+sin(x)-x*x; 
    } 
    
    x = 0.0; 
    result += 0.5*(exp(5.0*x)+sin(x)-x*x); 
    x = 1.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x); 

    return (h*result); 
}

double trapezoidal_parallel(int n) { 
    int comm_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    // Divide equal chunk of work
    int base_chunk = n / comm_size;
    int remainder = n % comm_size;

    double psum = 0.0, result; 
    double h = 1.0/n; 
    double x = rank*base_chunk; 
    int i, i_start, i_stop;

    if (rank==0) { // Compute first trapezoid
        psum += exp(5.0*x)+sin(x)-x*x;
    }

    if (comm_size - rank <= remainder) { // Compute remainders evenly among ranks
        i_start = rank*base_chunk + (remainder - (comm_size-rank))
        i_stop = (rank+1)*base_chunk + (remainder - (comm_size-rank-1))
    }
    
    else {
        i_start = rank*base_chunk;
        i_stop = (rank+1)*base_chunk;
    }

    for (i=i_start; i<i_stop; i++) {
        x += i*h;
        psum += exp(5.0*x)+sin(x)-x*x;
    }

    // x = 1.0 when i=n, but last worker won't calculate this since loop stops the index BEFORE i_stop=n => calculate as edgecase outside loop
    if (rank == comm_size-1) { // Don't need to be last rank, could be any rank
        x = 1.0 // Compute last trapezoid
        psum += exp(5.0*x)+sin(x)-x*x;
    } 

    MPI_Reduce(&psum, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return (h*result);    
}

int main(int argc, char *argv[]) {
    int n = 100;

    double result_serial = trapezoidal(n);

    MPI_Init(&argc, &argv);
    double result_parallel = trapezoidal_parallel(n);
    printf("Result parallel (n=%d): %lf\n", n, result_parallel);
    MPI_Finalize();

    printf("Result serial (n=%d): %lf\n", n, result_serial);
}