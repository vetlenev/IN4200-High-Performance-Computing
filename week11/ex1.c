// Measure overhead caused by point-to-point communication with MPI_Send and MPI_Recv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void some_work(int N) {
    double **A = malloc(N*N*sizeof *A);
    double *b = malloc(N*sizeof *b);
    double *x = calloc(N, sizeof *x);

    for (int i=0; i<N; i++) {
        b[i] = i % 10;
        for (int j=0; j<N; j++) {
            A[i][j] = 0.001*i + (N-j);
        }
    }

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            x[i] = x[i] + A[i][j]*b[j];
        }
    }
}

int main(int argc, char* argv[]) {
    int comm_size, rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 1e6, n_sends = 100;
    double *y = malloc(N*sizeof(double));
    double tot_time;

    if (rank == 0) {
        for (size_t i = 0; i < N; i++) {
            y[i] = rand();
        }
    }
    // Assume number of ranks (comm_size) is two
    // Measure ONLY time used for message transferal, not time used to process received message
    if (rank==0) {
        double time_start = MPI_Wtime(); // Only measure wall time for rank 0, since it will account for the send/receive from rank 1 as well (has to wait)
        for (int i=0; i<n_sends; i++) {
            MPI_Send(&y, N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&y, N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
        }
        double time_end = MPI_Wtime();
        tot_time = (time_end - time_start)/n_sends;
    }
    else {
        for (int i=0; i<n_sends; i++) {
            MPI_Recv(&y, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&y, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }

    if (rank==0) {
        printf("Total overhead time: %lf\n", tot_time);
        printf("NB: Calculated overhead time is slightly too much, because it also accounts for for-loop overhead.\n");
    }

    MPI_Finalize();
    printf("DONE\n");
    return 0;
}