// Parallel implementation of the trapezoidal function.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

double trapezoidal(int n);
void trapezoidal_parallel(double* result, int n,
                          int myrank, int nprocs, MPI_Comm comm);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int myrank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int n;
    if (myrank == 0) {
        if (argc < 2) {
            n = 42;
        } else {
            n = atoi(argv[1]);
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double parallel; // The result is put in this variable.
    trapezoidal_parallel(&parallel, n, myrank, nprocs, MPI_COMM_WORLD);

    if (myrank == 0) {
        double serial = trapezoidal(n);
        double diff = fabs(serial - parallel);
        printf("Serial:     %lf\n", serial);
        printf("Parallel:   %lf\n", parallel);
        printf("Difference: %lf\n", diff);
    }

    MPI_Finalize();
    return 0;
}

void trapezoidal_parallel(double* result, int n, int myrank, int nprocs,
                         MPI_Comm comm) {

    double local_result = 0.0;
    double h = 1.0/n;
    double x;
    int i;

    // Remember i=0 (and i=n) are not included in work division.
    int total_iters = n - 1;

    // Find start and stop of local interval.
    int displacement;
    int rest = total_iters%nprocs;
    int low_rank = nprocs - rest;
    // rank >= low_rank gets an extra iteration.
    int iters = total_iters/nprocs + (myrank >= low_rank ? 1:0);
    // All but the first of these are therefore displaced.
    if (myrank >= low_rank) {
        displacement = myrank - low_rank;
    } else {
        displacement = 0;
    }
    // Use this info to determine our interval.
    int i_start = 1 + myrank*(total_iters/nprocs) + displacement;
    int i_stop = i_start + iters;


    // Calculate intermediate results.
    for (i=i_start; i<i_stop; i++) {
        x = i*h;
        local_result += exp(5.0*x)+sin(x)-x*x;
    }
    printf("rank: %d, start: %d, stop: %d, local_result: %lf, x: %lf\n",
           myrank, i_start, i_stop, local_result, x);

    MPI_Reduce(&local_result,
               result,        // This is passed as a pointer to the function.
               1,
               MPI_DOUBLE,
               MPI_SUM,
               0,
               comm);

    // Let a single process handle edge cases and final division.

    if (myrank == 0) {
        printf("h = %lf\n", h);
        x = 0.0;
        *result += 0.5*(exp(5.0*x)+sin(x)-x*x);

        x = 1.0;
        *result += 0.5*(exp(5.0*x)+sin(x)-x*x);

        *result = (*result)*h;
    }
}

double trapezoidal(int n) {
    double result = 0.0;
    double h = 1.0/n;
    double x;
    int i;

    x = 0.0;
    for (i=1; i<n; i++) {
        x += h;
        result += exp(5.0*x)+sin(x)-x*x;
    }
    printf("%lf\n", result);

    x = 0.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    x = 1.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    return (h*result);
}
