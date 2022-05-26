#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "week10.h"

void option1(int rank, int comm_size, int n, double min, double max, MPI_Status status) {
    double psum = local_sum(rand(), n, min, max);
    double tot_sum;
    if (rank == 0) { // Master thread to collect and sum partial sums
        tot_sum = psum;
        for (size_t i=1; i<comm_size; i++) {
            MPI_Recv(&psum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            tot_sum += psum;
        }
        printf("Total sum: %lf\n", tot_sum);
    }
    else {
        MPI_Send(&psum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

void option2(int rank, int comm_size, int n, double min, double max) {

    double psum = local_sum(rand(), n, min, max);
    double *receiver;
    if (rank == 0) {
        // master process is the receiver, so initiate receiver as an array with one element for each partial sum (from other processes)
        receiver = malloc(comm_size*sizeof(double));
    }
    else {
        // other processes are not receivers, so just set to NULL
        receiver = NULL;
    }

    MPI_Gather(&psum, 1, MPI_DOUBLE, receiver, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); // passing address of psum will correctly distinguish the different partial sums

    if (rank == 0) {
        double tot_sum = psum; // first collect partial sum from master process
        tot_sum += sum_locals(receiver, comm_size); // the sum other partial sums
        printf("Total sum: %lf\n", tot_sum);
    }
}

void option3(int rank, int comm_size, int n, double min, double max, double *result) {
    double psum = local_sum(rand(), n, min, max);

    MPI_Reduce(&psum, result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total sum: %lf\n", *result);
    }
}

void option3new(int rank, int comm_size, int n, double min, double max, double *(*result)) {
    *result = calloc(comm_size, sizeof(double));
    double *psum = malloc(n*sizeof(double));
    for (int i=0; i<n; i++) {
        psum[i] = get_rand(rand(), min, max);
    }

    MPI_Reduce(&psum, *result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double tot_sum = 0;
        for (int i=0; i<n; i++) {
            tot_sum += (*result)[i];
        }
        printf("Total sum: %lf\n", **result);
    }
}

int main(int argc, char *argv[]) {

    int comm_size, rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double min = 0, max = 10;
    int n = 20;
    double result = 0;

    //option1(rank, comm_size, n, min, max, status);
    //option2(rank, comm_size, n, min, max);
    option3(rank, comm_size, n, min, max, &result);
    
    MPI_Finalize();

    return 0;
}