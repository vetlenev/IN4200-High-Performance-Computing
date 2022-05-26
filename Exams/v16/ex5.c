#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>

void para_matvec(int n, double **A, double *x, double *y) {
    MPI_Bcast(&x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // Iterations in computations are independent -> no need for communication
    my_start = my_rank*n/num_procs;
    my_stop = (my_rank+1)*n/num_procs;
    for (int i=my_start; i<my_stop; i++) {
        for (int j=0; j<n; j++) {
            y[i] += A[i][j] * x[j];
        }
    }

    int *n_rows = malloc(num_procs*sizeof n_rows);
    int *displ_y = malloc(num_procs*sizeof displ_y);
    displ_y[0] = 0;
    for (int rank=0; rank<num_procs-1; rank++) {
        n_rows[rank] = (n/num_procs) + ((rank < (n%num_procs))?1:0);
        displ_y[rank+1] = displ_y[rank] + n_rows[rank];
    }
    n_rows[num_procs-1] = (n/num_procs);

    MPI_Gatherv(y,
                n_rows[my_rank],
                MPI_DOUBLE,
                y,
                n_rows,
                displ_y,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    // MPI_Reduce would produce wrong results here, because we then sum together the n elements
    // of global vector y, but the results are already "ready" for each process.
}

int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    int n = 100;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double **A;
    double *x;
    double *y;

    if (my_rank == 0) {
        A = malloc(n*sizeof *A);
        x = malloc(n*sizeof x);
        A[0] = malloc(n*n*sizeof A);

        for (int i=1; i<n; i++) {
            A[i] = &A[0][i*n];
        }

        for (int i=0; i<n; i++) {
            x[i] = i%4 - j%3;
            for (int j=0; j<n; j++) {
                A[i][j] = 0.01*i + 0.02*j
            }
        }
    }

    MPI_Bcast(&x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double **my_A;
    double *my_y;

    int *sendcounts = malloc(num_procs*sizeof sendcounts);
    int *n_rows = malloc(num_procs*sizeof n_rows);
    int *displ_A = malloc(num_procs*sizeof displ_A); // scattering and gathering is done for same array -> only need one displ
    int *displ_y = malloc(num_procs*sizeof displ_y);
    displ_A[0] = 0;
    displ_y[0] = 0;
    for (int rank=0; rank<num_procs-1; rank++) {
        n_rows[rank] = (n/num_procs) + ((rank < (n%num_procs))?1:0);
        sendcounts[rank] = n_rows[rank]*n; // assign all columns
        displ_A[rank+1] = displ_A[rank] + sendcounts[rank];
        displ_y[rank+1] = displ_y[rank] + n_rows[rank];
    }
    n_rows[num_procs-1] = (n/num_procs);
    sendcounts[num_procs-1] = n_rows[num_procs-1]*n;

    my_A = malloc(n_rows[my_rank]*sizeof *my_A);
    my_A[0] = malloc(n_rows[my_rank]*n*sizeof my_A);
    for (int i=1; i<n_rows[my_rank], i++) {
        my_A[i] = &my_A[0][i*n];
    }
    my_y = malloc(n_rows[my_rank]*sizeof my_y);

    MPI_Scatterv(A,
                sendcounts,
                displ_A,
                MPI_DOUBLE,
                my_A,
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    MPI_Scatterv(y,
                n_rows,
                displ_y,
                MPI_DOUBLE,
                my_y,
                n_rows[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    
}