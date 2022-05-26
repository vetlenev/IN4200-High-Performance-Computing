// MPI implementation of a distributed matrix-vector multiplication.
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "week10.h"

// Used to index local copies of the data in matrix A ex. 4.
#define idx2(i,j) (i*n_cols[myrank] + j)

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
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); // Copy address of N to all processes -> each process can now access its own N

    // Calculate displacements and number of rows for each process.
    int *n_cols = malloc(numprocs*sizeof *n_cols);
    int *displs = malloc(numprocs*sizeof *displs);

    int cols = N/numprocs;
    int remainder = N%numprocs;

    // Last remainder processes gets an extra row.
    displs[0] = 0;
    for (int rank = 0; rank < numprocs-1; rank++) {
        n_cols[rank] = cols + ((rank >= (numprocs - remainder)) ? 1:0);
        displs[rank+1] = displs[rank] + n_cols[rank];
    }
    n_cols[numprocs-1] = cols + ((numprocs-1) >= (numprocs - remainder) ? 1:0);

    // Global buffer must be defined for all processes even though only rank 0 uses them, because MPI_Scatterv and MPI_Reduce are collective methods called by ALL processes -> so if A, x or y is not defined for a given process, calling these will return error
    double *A;
    double *x;
    double *y;
    if (myrank == 0) {
        A = malloc(N*N*sizeof(double));
        x = malloc(N*sizeof(double));
        y = malloc(N*sizeof(double));

        for (size_t i = 0; i < N; i++) {
            x[i] = i%4; // each process have ALLOCATED array x, but only rank 0 INITIALIZES it
            for (size_t j = 0; j < N; j++) {
                A[i*N + j] = 0.01*i + 0.01*j;
            }
        }   
    }

    // Allocate local buffers --> rank 0 now has both a local version of A and a full/global version
    double *local_A = malloc(N*n_cols[myrank]* sizeof *local_A); // A is to be accessed column-wise -> no need to make it contiguous ??
    double *local_x = malloc(n_cols[myrank]* sizeof *local_x); // since work is assigned column-wise, not all elements of x will be used for each worker
    double *local_y = calloc(N, sizeof *local_y);
    // No initialization of A and x -> its values are scattered from rank 0
    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes, to make sure no computation is done before all arrays are allocated?

    MPI_Datatype col_temp, col_vec, recv_temp, recv_col;

    MPI_Type_vector(N, 1, N, MPI_DOUBLE, &col_temp); // Vector datatype to send columns of A
    MPI_Type_create_resized(col_temp, 0, sizeof(double), &col_vec); // Converted to a datatype where extent has been reduced to only one double-element, allowing first element of next vector to be placed right after first element of current vector
    MPI_Type_commit(&col_vec); // Commit the new datatype of col_vec to make it applicable

    MPI_Type_vector(N, 1, n_cols[myrank], MPI_DOUBLE, &recv_temp); // Vector datatype to send elements of x that corresponds to given column of A (for the datatype col_vec)
    MPI_Type_create_resized(recv_temp, 0, sizeof(double), &recv_col);
    MPI_Type_commit(&recv_col);

    MPI_Scatterv(A,             // address of buffer/data to send out (only relevant for rank 0)
                 n_cols,        // Amount of data to send TO EACH PROCESSOR
                 displs,        // displacement specifying startposition of buffer to send out data from
                 col_vec,       // type of data to send -> own specified type since we must return non-contiguous elements
                 local_A,       // receive buffer of each process
                 n_cols[myrank],// how much data to receive by each processor
                 recv_col,      // datatype for receiver
                 0,             // root process to send out data
                 MPI_COMM_WORLD);

    // Need also to send separate elements of x
    MPI_Scatterv(x,
                 n_cols,
                 displs,
                 MPI_DOUBLE,
                 local_x,
                 n_cols[myrank],
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);

    // Computation
    for (int i=0; i<N; i++) { // Each process has at least one elemet from each row
        for (int j=0; j<n_cols[myrank]; j++) { // Only traverse relevant column for this process
            local_y[i] += local_A[i*n_cols[myrank] + j] * local_x[j];
        }
    }

    // After computation - use elementwise reduction instead of gathering all results
    MPI_Reduce(&local_y, &y, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // local_y is send bufer, y is receive buffer, N elements are sent from each element (remember all processes are assigned at least one column of A)

    if (myrank == 0) { // rank 0 is the process that receives the reduced result, thus is the one that must do printing
        printf("Result of column-wise work allocation:\n");
        printf("y = \n");
        printvec(y, N);
    }

    free(local_x);
    free(local_A);
    free(local_y);

    free(x);
    free(A);
    free(y);

    MPI_Finalize();

    return 0;
}

    