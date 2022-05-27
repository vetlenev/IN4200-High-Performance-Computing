#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "utility.h"

double **MMM_serial(int N) {
    double **A = malloc(N*sizeof *A);
    double **B = malloc(N*sizeof *B);
    double **C = malloc(N*sizeof *C);
    A[0] = malloc(N*N*sizeof A);
    B[0] = malloc(N*N*sizeof B);
    C[0] = malloc(N*N*sizeof C);
    //read_matrix_binaryformat("small_matrix_a.bin", &A, &m, &l);
    //read_matrix_binaryformat("small_matrix_b.bin", &B, &l, &n);
    //read_matrix_binaryformat("small_matrix_c.bin", &C, &m, &n);

    for (int i=1; i<N; i++) {
        A[i] = &A[0][i*N];
        B[i] = &B[0][i*N];
        C[i] = &C[0][i*N];
    }

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            A[i][j] = 0.01*(i%2) + 0.01*(j%10);
            B[i][j] = 0.01*i + 0.01*(j%4);
        }
    }

    double s;
    for (int i=0; i<N; i++) { 
        for (int j=0; j<N; j++) {
            s = 0.0;
            for (int k=0; k<N; k++) {
                s += A[i][k]*B[k][j];
            }
            C[i][j] = s;
        }
    }

    return C;
}

double **MMM_omp(int N) {
    double **A = malloc(N*sizeof *A);
    double **B = malloc(N*sizeof *B);
    double **C = malloc(N*sizeof *C);
    A[0] = malloc(N*N*sizeof A);
    B[0] = malloc(N*N*sizeof B);
    C[0] = malloc(N*N*sizeof C);

    for (int i=1; i<N; i++) {
        A[i] = &A[0][i*N];
        B[i] = &B[0][i*N];
        C[i] = &C[0][i*N];
    }

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            A[i][j] = 0.01*(i%2) + 0.01*(j%10);
            B[i][j] = 0.01*i + 0.01*(j%4);
        }
    }

    int i, j, k;
    double s;
    /* To avoid race condition on the elements of C,
    we need to update each factor k of a particular C[i][j] separately.
    => the summation over k must be done serially for each element C[i][j],
    and then Reduced in the end.
    */
    #pragma omp parallel for private(j) private(k) reduction(+:s)
    for (i=0; i<N; i++) { 
        for (j=0; j<N; j++) {
            s = 0.0;
            for (k=0; k<N; k++) {
                s += A[i][k]*B[k][j]; // OpenMP does reduction automatically here ???
                // This is a reduction operation, but each thread only updates to its own C[i][j] value,
                // because there are no dependencies across indices owned by different threads.
            C[i][j] = s;
        }
    }
    }

    return C;
}

double **MMM(int my_rank, int num_procs) {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double **A, **B, **C;
    int m; // nr rows A
    int l; // nr cols A = nr rows B
    int n; // nr cols B

    if (my_rank == 0) {
        // Only rank 0 initializes matrices
        read_matrix_binaryformat("small_matrix_a.bin", &A, &m, &l);
        read_matrix_binaryformat("small_matrix_b.bin", &B, &l, &n);
        read_matrix_binaryformat("small_matrix_c.bin", &C, &m, &n);
    }

    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int P = sqrt(num_procs); // number of blocks in horizontal/vertical direction
    /* 
    // Partition A matrix
    int bounds_A = block2D(my_rank, m, l, P, P);
    int x_start_A = bounds_A[0], x_stop_A = bounds_A[1];
    int y_start_A = bounds_A[2], y_stop_A = bounds_A[3];
    // Partition B matrix
    int bounds_B = block2D(my_rank, l, n, P, P);
    int x_start_B = bounds_B[0], x_stop_B = bounds_B[1];
    int y_start_B = bounds_B[2], y_stop_B = bounds_B[3];
    */

    int *sendcounts_A = malloc(num_procs*sizeof sendcounts_A);
    int *sendcounts_B = malloc(num_procs*sizeof sendcounts_B);
    int *n_rows_A = malloc(num_procs*sizeof n_rows_A);
    int *n_cols_A = malloc(num_procs*sizeof n_cols_A);
    int *n_rows_B = malloc(num_procs*sizeof n_rows_B);
    int *n_cols_B = malloc(num_procs*sizeof n_cols_B);
    int *displ_A = malloc(num_procs*sizeof displ_A);
    int *displ_B = malloc(num_procs*sizeof displ_B);
    displ_A[0] = 0;
    displ_B[0] = 0;

    MPI_Datatype block_A_temp, block_A, block_B_temp, block_B, block_C_temp, block_C;

    for (int rank=0; rank<num_procs-1; rank++) {
        int *bounds_A = block2D(rank, m, l, P, P);
        int x_start_A = bounds_A[0], x_stop_A = bounds_A[1];
        int y_start_A = bounds_A[2], y_stop_A = bounds_A[3];
        int *bounds_B = block2D(rank, l, n, P, P);
        int x_start_B = bounds_B[0], x_stop_B = bounds_B[1];
        int y_start_B = bounds_B[2], y_stop_B = bounds_B[3];
        int x_start_C, x_stop_C, y_start_C, y_stop_C;

        n_rows_A[rank] = x_stop_A - x_start_A;
        n_cols_A[rank] = y_stop_A - y_start_A;
        n_rows_B[rank] = x_stop_B - x_start_B;
        n_cols_B[rank] = y_stop_B - y_start_B;

        sendcounts_A[rank] = n_rows_A[rank]*n_cols_A[rank];
        sendcounts_B[rank] = n_rows_B[rank]*n_cols_B[rank];

        displ_A[rank+1] = displ_A[rank] + sendcounts_A[rank]; // WRONG - continue here

        // nr block: x_stop_A-x_start_A. blocksize: y_stop_A-y_start_A. stride: l 
        MPI_Type_vector(x_stop_A-x_start_A, y_stop_A-y_start_A, l, MPI_DOUBLE, &block_A_temp);
        MPI_Type_create_resized(block_A_temp, 0, sizeof(double), &block_A);
        MPI_Type_commit(&block_A);

        MPI_Type_vector(x_stop_B-x_start_B, y_stop_B-y_start_B, l, MPI_DOUBLE, &block_B_temp);
        MPI_Type_create_resized(block_B_temp, 0, sizeof(double), &block_B);
        MPI_Type_commit(&block_B);

        MPI_Type_vector(x_stop_C-x_start_C, y_stop_C-y_start_C, l, MPI_DOUBLE, &block_C_temp);
        MPI_Type_create_resized(block_C_temp, 0, sizeof(double), &block_C);
        MPI_Type_commit(&block_C);
    }

    double **my_A = malloc(n_rows_A[my_rank]*sizeof *my_A); 
    my_A[0] = malloc(sendcounts_A[my_rank]*sizeof my_A);
    for (int i=1; i<n_rows_A[my_rank]; i++) {
        my_A[i] = &my_A[0][i*n_cols_A[my_rank]];
    }

    MPI_Scatterv(A,
                sendcounts_A,
                displ_A,
                block_A, // type used to send
                my_A,
                sendcounts_A[my_rank],
                block_C, // type used to receive
                0,
                MPI_COMM_WORLD);
    
    return C;
}

int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    int N = 10;
    //MPI_Init(&argc, &argv);
    //double **D;
    //D = MMM(my_rank, num_procs);
    double **C_s = MMM_serial(N);
    double **C_p = MMM_omp(N);

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            if (C_s[i][j] != C_p[i][j]) {
                printf("Element C[%d][%d] not equal: %lf != %lf\n", i, j, C_s[i][j], C_p[i][j]);
                return 1;
            }
        }
    }

    return 0;
}