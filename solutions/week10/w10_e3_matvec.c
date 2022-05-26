// MPI implementation of a distributed matrix-vector multiplication.
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "set10.h"

int main(int argc, char *argv[]) {
    int myrank, numprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // cmd line arguments in MPI programs depend on the specific implementation
    // you are using. The safest way to make sure all processes gets the
    // arguments is to broadcast them.
    int N;
    if (myrank == 0) {
        if (argc>1) {
            N = atoi(argv[1]);
        } else {
            N = numprocs*3 + 1;
        }
        printf("N = %d\n", N);
        printf("numprocs = %d\n", numprocs);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate displacements and number of rows for each process.
    int *n_rows = malloc(numprocs*sizeof *n_rows);

    // Used when scattering A.
    int *sendcounts = malloc(numprocs*sizeof *sendcounts);
    int *Sdispls = malloc(numprocs*sizeof *Sdispls);

    // Used when gathering y.
    int *Gdispls = malloc(numprocs*sizeof *Gdispls);


    int rows = N/numprocs;
    int remainder = N%numprocs;
    Sdispls[0] = 0;
    Gdispls[0] = 0;

    // Last remainder processes gets an extra row.
    for (int rank = 0; rank < numprocs-1; rank++) {
        n_rows[rank] = rows + ((rank >= (numprocs - remainder)) ? 1:0); // Assigns extra row (1) to this rank if all even rows have been distributed
        sendcounts[rank] = n_rows[rank]*N; // number of elements to send
        Sdispls[rank+1] = Sdispls[rank] + sendcounts[rank]; // number of array elements that has been sent for previous processes
        Gdispls[rank+1] = Gdispls[rank] + n_rows[rank]; // row number for this process, used as identifier for gathering result
    }
    n_rows[numprocs-1] = rows + ((numprocs-1) >= (numprocs - remainder) ? 1:0);

    sendcounts[numprocs-1] = n_rows[numprocs-1]*N;

    // Allocate local buffers.
    double *A;
    double *x = malloc(N * sizeof *x); // Similar allocation to each process - everyone needs entire vector
    if (myrank == 0) {
        A = malloc(N*N * sizeof *A); // only rank 0 allocates A -> its content to be scattered evenly to other processes

        // Initialize to some values:
        for (size_t i = 0; i < N; i++) {
            x[i] = i%4; // each process have ALLOCATED array x, but only rank 0 INITIALIZES it
            for (size_t j = 0; j < N; j++) {
                A[idx(i,j)] = 0.01*i + 0.01*j;
            }
        }
    } else {
        A = malloc(N*n_rows[myrank] * sizeof *A); // only allocate parts of A that are to be computed by this process
    }

    // Broadcast x and scatter A.
    // x: BROADCASTED because all processes should receive same (full) vector x
    // A: SCATTERED because different processes will work on different parts
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(A,                 // Sendbuff, matters only for root process.
                 sendcounts,
                 Sdispls,           // The array location/index of original sendbuf (A) from which to send data to receiver process (fetch sendcounts elements from Sdispls index)
                 MPI_DOUBLE,
                 A,                 // Recieve buff is the same as sendbuf here.
                 N*n_rows[myrank],  // Number of elements in receiver process
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);


    // Actual matrix-vector multiplication
    double *y;
    if (myrank == 0) {
        y = malloc(N * sizeof *y); // We are going to gather in to this buffer.
    } else {
        y = malloc(n_rows[myrank] * sizeof *y); // resulting vector to be sent back to rank 0
    }

    for (size_t i = 0; i < n_rows[myrank]; i++) {
        y[i] = 0;
        for (size_t j = 0; j < N; j++) {
            y[i] += A[idx(i,j)] * x[j];
        }
    }

    // Gather the results
    // NB: Actual gathering only happens for rank 0, but the function anyway needs to be called by other ranks, to specify the buffer and corresponding location that is to be gathered from each process
    MPI_Gatherv(y,
                n_rows[myrank], // Number of elements to send back
                MPI_DOUBLE,
                y,              // Matters only at root,
                n_rows,         // Number of elements from this process to be receives by rank 0
                Gdispls,        // Array of addresses in receiver to place the incoming data
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    // Print the results and compare.
    if (myrank == 0) {
        double *y2 = malloc(N * sizeof *y2);
        matvec_mult(A, x, y2, N);

        if (N < 21) {
            printf("MPI results:\n");
            printvec(y, N);

            printf("Serial results:\n");
            printvec(y2, N);
        }

        double error = sum_err_sqr(y, y2, N);
        printf("Sum error squared:\n");
        printf("%lf\n", error);

        free(y2);
    }

    free(A);
    free(x);
    free(y);
    free(n_rows);
    free(Sdispls);
    free(Gdispls);
    free(sendcounts);

    MPI_Finalize();

    return 0;
}
