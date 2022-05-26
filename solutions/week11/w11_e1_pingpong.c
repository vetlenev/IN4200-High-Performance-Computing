// MPI Ping Pong test

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "set11.h"

int main(int argc, char *argv[]) {
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (nprocs != 2) {
        if (myrank == 0) {
            printf("This program should be run with only two processes:\n");
            printf("$ mpirun -n 2 ./w11_e1_pingpong.exe [N]\n");
        }
        MPI_Finalize();
        return 1;
    }

    int N;
    if (myrank == 0) {
        if (argc < 2) {
            N = 3e7; // Equals 30Mb of data in chars.
        } else {
            N = atoi(argv[1]);
        }
    }

    // This broadcast is used to make the code more robust.
    // All MPI implementations try to make argv and argc available to all ranks.
    // But no gurantee is given.
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Generate some random data.
    char *data = malloc(N*sizeof *data);
    int n = 100;
    int *n_values = linspace(1, N, n);

    double *time;
    if (myrank == 0) {
        for (size_t i = 0; i < N; i++) {
            data[i] = rand();
        }
        time = malloc(n * sizeof *time);
    }

    // Ping pong loop.
    int n_trials = 25;
    for (size_t i = 0; i < n; i++) {

        if (myrank == 0) {
            printf("\r%ld/%d, message size: %d bytes.", i+1, n,
                                                              n_values[i]);
            fflush(stdout);
            double start = MPI_Wtime();
            for (size_t j = 0; j < n_trials; j++) {
                MPI_Send(data, n_values[i], MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(data, n_values[i], MPI_CHAR, 1, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE); // MPI_STATUS_IGNORE may or may not
                                            // be NULL
            }
            double end = MPI_Wtime();
            time[i] = (end - start)/n_trials;
        } else {
            for (size_t j = 0; j < n_trials; j++) {
                MPI_Recv(data, n_values[i], MPI_CHAR, 0, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                MPI_Send(data, n_values[i], MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

    }

    // Write results to file.
    if (myrank == 0) {
        printf("\n"); // Here so the output to the console looks pretty.
        FILE *fp = fopen("w11_e1.dat", "w");
        for (size_t i = 0; i < n; i++) {
            fprintf(fp, "%.17g, %.17g\n", (double)n_values[i]/1000,
                                          time[i]*1000);
        }
        fclose(fp);

        free(time);
    }

    free(data);
    free(n_values);

    MPI_Finalize();

    return 0;
}
