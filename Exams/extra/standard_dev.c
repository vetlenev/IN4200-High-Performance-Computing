#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "utility.h"

int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    int n = 1000;
    double *a_rand = malloc(n*sizeof a_rand);
    double *a_s = malloc(n*sizeof a_s);

    // MPI_START
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_rank == 0) {
        // SERIAL CASE
        for (int i=0; i<n; i++) {
            a_rand[i] = randfrom(-1.0, 1.0);
        }

        double simple_sum_s;
        for (int i=0; i<n; i++) {
            simple_sum_s += a_rand[i];
        }
        
        double average_s = simple_sum_s / n;

        double squared_sum_s = 0;
        for (int i=0; i<n; i++) {
            squared_sum_s += (a_rand[i] - average_s)*(a_rand[i] - average_s);
        }

        double sigma_s = sqrt(squared_sum_s / n);
        printf("Standard dev: %lf\n", sigma_s);
    }

    int my_start = my_rank*n/num_procs;
    int my_stop = (my_rank+1)*n/num_procs;
    int my_count = my_stop - my_start;
    
    int base = n / num_procs;
    int rem = n % num_procs;
    int *sendcount = malloc(num_procs*sizeof sendcount);
    int *displ = malloc(num_procs*sizeof displ);

    displ[0] = 0;
    for (int rank=0; rank<num_procs; rank++) {
        sendcount[rank] = base + ((rank<rem)?1:0);
        if (rank < num_procs-1)
            displ[rank+1] = displ[rank] + sendcount[rank];
    }

    double *my_a = malloc(sendcount[my_rank]*sizeof my_a);
    //MPI_Bcast(&a_rand, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(a_rand,
                 sendcount,
                 displ,
                 MPI_DOUBLE,
                 my_a,
                 sendcount[my_rank],
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);

    // Each rank initializes its part of array a
    double individual_sum = 0;
    for (int i=0; i<sendcount[my_rank]; i++) {
        //my_a[i] = a_rand[i];
        individual_sum += my_a[i];
    }

    double simple_sum;
    // Using MPI_Allreduce, we avoid having to broadcast the average result to each processor
    MPI_Allreduce(&individual_sum, &simple_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    
    double average = simple_sum / n;

    double my_squared_sum = 0;
    for (int i=0; i<sendcount[my_rank]; i++) {
        // deliberately multiply twice than using pow to avoid overhead
        my_squared_sum += (my_a[i] - average)*(my_a[i] - average);
    }

    double squared_sum;
    MPI_Reduce(&my_squared_sum, &squared_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double sigma;
    if (my_rank == 0) {
        sigma = sqrt(squared_sum/n);
        printf("Standard dev MPI: %lf\n", sigma);
    }

    MPI_Finalize();
    return 0;
}