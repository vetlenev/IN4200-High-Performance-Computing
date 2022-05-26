#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "week10.h"

void copySendBuf_A(double **A, double *(**sbufA), int i_start, int i_stop, int N) {
    // Store sent buffer of A intermediately
    for (int i=i_start; i<i_stop; i++) {
        for (int j=0; j<N; j++) {
            (*sbufA)[i][j] = A[i][j];
        }
    }
}

void copySendBuf_x(double *x, double *(*sbufx), int i_start, int i_stop) {
    // Store sent buffer of x intermediately
    for (int i=0; i<N; i++) {
        (*sbufx)[i] = x[i]; // store entire x
    }
}

void copyRecvBuf_A(double *(**A), double **rbufA, int i_start, int i_stop, int N) {
    // Copy received message to intermediate storage
    for (int i=i_start; i<i_stop; i++) {
        for (int j=0; j<N; j++) {
            (*A)[i][j] = rbufA[i][j];
        }
    }
}

void copyRecvBuf_x();

void copySendBuf_y();

void copyRecvBuf_y();

int main(int argc, char *argv[]) {
    /* For column-wise work distribution of A, the data for 
    each process is not contiguously located.
    When sending initial work from rank 0 to other ranks,
    we must provide an intermediate storage of the data
    in the form of a buffer.
    --> Need to make use of CopySendBuf and 
        CopyRecvBuf functions for temporary storage.
    */
    int N = 1000;
    int P = 8;

    int remainder_rows = N % P; // calculate before MPI-region to avoid redundant calculations
    int d_idx = N\P; // assigned row interval

    // Initialize MPI and cartesian topology
    int comm_size, rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request *request_array;
    MPI_Request req_A;
    MPI_Request req_x;
    MPI_Status *status_array;
    MPI_Status status_A;
    MPI_Status status_x;

    // Divide work evenly among processors P
    if (remainder_rows == 0) { // work load fits exactly in domain
        int i_start = rank*d_idx;
        int i_stop = (rank+1)*d_idx;
    }
    else {
        if (rank < P-remainder_rows) { // assign N\P rows for the first P-remainder_rows processes
            int i_start = rank*d_idx;
            int i_stop = (rank+1)*d_idx;
        }
        else if (remainder_rows == P-remainder_rows) {
            int i_start = rank*d_idx;
            int i_stop = (rank+1)*(d_idx+1);
        }
        else { // Assign extra row to last remainder_rows
            int i_start = rank*(d_idx+1);
            int i_stop = (rank+1)*(d_idx+1);
        }
    }
    int i_size = i_stop - i_start;

    if (rank == 0) {
        // Rank zero contains full arrays
        double **iA = (double**)malloc(N*sizeof(double*));
        double *ix = (double*)malloc(N*sizeof(double));
       
        iA[0] = (double*)malloc(N*N*sizeof(double));

        for (int i=1; i<N; i++) {
            iA[i] = &(iA[0][i*N]);
        }

        for (int i=0; i<N; i++) {
            ix[i] = (double)i/N;
            for (int j=0; j<N; j++) {
                iA[i][j] = (double)(i+j);
            }       
        }
    }

    if (rank == 0) {

        for (int ir=1; ir<P-remainder_rows; ir++) {
            // Send local chuncks to other processes
            copySendBuf_A(iA, &sbuf_A, ir*d_idx, (ir+1)*d_idx, N); // sbuf0 is 2D array
            MPI_Send(sbuf_A, d_idx, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD);
            copySendBuf_x(ix, &sbuf_x, 0, N); // sbuf0 is 2D array
            MPI_Send(&ix, N, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD);
            // Receive local computed parts
            MPI_Irecv(&iy[ir*d_idx], d_idx, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD, &request_array[ir]);
        }
        // First remainder row
        MPI_Send(&iA[(P-remainder_rows)*d_idx], d_idx+1, MPI_DOUBLE, P-remainder_rows, 0, MPI_COMM_WORLD);
        MPI_Send(&ix, N, MPI_DOUBLE, P-remainder_rows, 0, MPI_COMM_WORLD);
        MPI_Irecv(&iy[(P-remainder)*d_idx], d_idx+1, MPI_DOUBLE, P-remainder_rows, 0, MPI_COMM_WORLD, &request_array[P-remainder_rows]);
        // Subsequent remainder rows
        for (int ir=P-remainder_rows+1; ir<P; ir++) { // Send and receive the remaining rows that are larger
            MPI_Send(&iA[ir*(d_idx+1)], d_idx+1, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD);
            MPI_Send(&ix, N, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD);
            MPI_Irecv(&iy[ir*(d_idx+1)], d_idx+1, MPI_DOUBLE, ir, 0, MPI_COMM_WORLD, &request_array[ir]);
        }
    }
    else { // receive chunk of A and x, do local computation, send back to process 0
        MPI_Irecv(&iA[i_start], i_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &req_A);
        MPI_Irecv(&ix, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &req_x);
        // Send local computed result back to rank 0
        MPI_Send(&iy[i_start], i_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        MPI_Waitall(P-1, request_array, status_array);
        // Assemble all y (should it be done AFTER the computations ??)
    }
    else {
        MPI_Wait(&req_A, &status_A);
        MPI_Wait(&req_x, &status_x);
    }

    for (int i=i_start; i<i_stop; i++) {
        for (int j=0; j<N; j++) {
            iy[i] = iy[i] + iA[i][j]*ix[j];
        }
    }

    free(iy);
    free(iA);
    free(ix);

    MPI_Finalize();

    return 0;
}