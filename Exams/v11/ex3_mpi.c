#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

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

    x = 0.;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    x = 1.0;
    result += 0.5*(exp(5.0*x)+sin(x)-x*x);

    return (h*result);
}

double trapezoidal_mpi (int n, int my_rank, int num_procs) {
    double psum = 0.0;
    double h = 1.0/n;

    int my_start = my_rank*n/num_procs;
    int my_stop = (my_rank+1)*n/num_procs;
    double x;

    if (my_rank == 0) {
        x = 0.;
        psum += 0.5*(exp(5.0*x)+sin(x)-x*x);
        x = 1.0;
        psum += 0.5*(exp(5.0*x)+sin(x)-x*x);
        my_start = 1;
    }

    for (int i=my_start; i<my_stop; i++) {
        x = i*h;
        psum += exp(5.0*x)+sin(x)-x*x;
    }

    return (h*psum);
}

int main(int argc, char *argv[]) {
    int n = 950;
    int my_rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int base = n / num_procs;
    int rem = n % num_procs;

    /*
    int *sendcounts = malloc(num_procs*sizeof sendcounts);
    int *displ = malloc(num_procs*sizeof displ);
    displ[0] = 0
    for (int rank=0; rank<num_procs-1; rank++) {
        sendcounts[rank] = base + ((rank<rem)?1:0);
        displ[rank+1] = displ[rank] + sendcounts[rank];
    }
    sendcounts[num_procs-1] = base + ((num_procs<rem)?1:0);
    */

    double psum;
    psum = trapezoidal_mpi(n, my_rank, num_procs);
    double tot_sum;

    MPI_Reduce(&psum, &tot_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        double sum_serial = trapezoidal(n);
        printf("Sum serial: %lf\n", sum_serial);
        printf("Sum mpi: %lf\n", tot_sum);
    }

    MPI_Finalize();
    return 0;
}