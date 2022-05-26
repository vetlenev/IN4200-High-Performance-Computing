#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define _USE_MATH_DEFINES

double *serial(int N, double a, double dt, double final_T) {
    double *u=(double*)malloc((N+2)*sizeof(double));
    double *u_prev=(double*)malloc((N+2)*sizeof(double));
    double *tmp_ptr;
    for (int i=0; i<=N+1; i++)
        u_prev[i]=sin(M_PI*i/(N+1));

    double t = 0.0;
    while (t < final_T) {
        for (int i=1; i<=N; i++)
            u[i]=u_prev[i]+a*(u_prev[i-1]-2*u_prev[i]+u_prev[i+1]);

        u[0] = 0.0;
        u[N+1] = 0.0;

        t += dt;
        tmp_ptr = u_prev;
        u_prev = u;
        u = tmp_ptr;
    }
    return u_prev; // the newly updated one
}

double *parallel_mpi(int N, double a, double dt, double final_T, int my_rank, int num_procs) {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double *u; 
    double *u_prev;
    double t;

    if (my_rank == 0) {
        u = malloc((N+2)*sizeof(double));
        u_prev = malloc((N+2)*sizeof(double));
        for (int i=0; i<=N+1; i++)
            u_prev[i]=sin(M_PI*i/(N+1));
        t = 0.0;
    }

    MPI_Bcast(&t, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double *my_u;
    double *my_u_prev;

    int *sendcounts = malloc(num_procs*sizeof sendcounts);
    int *displ = malloc(num_procs*sizeof displ);
    displ[0] = 0;
    for (int rank=0; rank<num_procs-1; rank++) {
        sendcounts[rank] = (N)/num_procs + ((rank < (N)%num_procs)?1:0); // include boundary points ??? (THIS IS NOT GHOSTS)
        displ[rank+1] = displ[rank] + sendcounts[rank];
    }
    sendcounts[num_procs-1] = N/num_procs; // remainders are distributed among first processors => impossible for last processor to get a remainder

    int my_N = sendcounts[my_rank];
    my_u = malloc((my_N+2)*sizeof my_u); // +2 to hold ghost points
    my_u_prev = malloc((my_N+2)*sizeof my_u_prev);

    if (my_rank == 0) // Assign left global boundary to left ghost of first process (fixed)
        my_u_prev[0] = 0.0;
    
    if (my_rank == num_procs-1) // Assign right global boundary to right ghost of last process (fixed)
        my_u_prev[my_N+1] = 0.0;     

    MPI_Scatterv(u + 1, // +1 to omit global boundaries - don't require computations
                sendcounts,
                displ,
                MPI_DOUBLE,
                my_u + 1,
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD); // +1 since first is reserved for ghost point

    MPI_Scatterv(u_prev + 1, // +1 to omit global boundaries (don't require computations)
                sendcounts,
                displ,
                MPI_DOUBLE,
                my_u_prev + 1,
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    double *tmp_ptr = malloc((my_N+2)*sizeof tmp_ptr);
     
    while (t < final_T) {
        // Send/receive data BEFORE computations
        MPI_Request request_from_right;
        MPI_Request request_from_left;
        MPI_Status status_left;
        MPI_Status status_right;
        double left_ghost; // initialized just as emtpy address - to be filled when neighbor value is received
        double left_boundary = my_u_prev[1];
        double right_boundary = my_u_prev[my_N];
        double right_ghost;
        
        if (my_rank % 2 != 0) { // odd ranks send first
            if (my_rank != num_procs-1)
                MPI_Send(&right_boundary, 1, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD);

            MPI_Send(&left_boundary, 1, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD); // odd nodes can always send left

            if (my_rank != num_procs-1) {
                MPI_Irecv(&right_ghost, 1, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, &request_from_right);
                MPI_Wait(&request_from_right, &status_right);
                my_u_prev[my_N+1] = right_ghost;
            }

            MPI_Irecv(&left_ghost, 1, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &request_from_left);
            MPI_Wait(&request_from_left, &status_left);
            my_u_prev[0] = left_ghost;
        }
        else {
            if (my_rank != num_procs-1) {
                MPI_Irecv(&right_ghost, 1, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, &request_from_right);
            }
            if (my_rank != 0) {
                MPI_Irecv(&left_ghost, 1, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &request_from_left);
            } // Wait necessary to ensure communication is finished before doing computations

            if (my_rank != num_procs-1) {
                MPI_Send(&right_boundary, 1, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD);
            }
            if (my_rank != 0) {
                MPI_Send(&left_boundary, 1, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD);
            }
            if (my_rank != num_procs-1) {
                MPI_Wait(&request_from_right, &status_right);
                my_u_prev[my_N+1] = right_ghost;
            }
            if (my_rank != 0) {
                MPI_Wait(&request_from_left, &status_left);
                my_u_prev[0] = left_ghost;
            }
        }

        for (int i=1; i<=my_N; i++)
            my_u[i] = my_u_prev[i] + a*(my_u_prev[i-1]-2*my_u_prev[i]+my_u_prev[i+1]);

        t += dt; // Avoid race condition and proper time update by only letting one rank increment time ???

        // As opposed to OpenMP, pointer swap needs to be done for all ranks, since local arrays are different on each processor
        tmp_ptr = my_u_prev;
        my_u_prev = my_u;
        my_u = tmp_ptr;
    }

    MPI_Gatherv(my_u_prev+1, // send from u_prev - the newly updated one
                sendcounts[my_rank],
                MPI_DOUBLE,
                u+1,
                sendcounts,
                displ,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);
    
    // free local arrays
    free(sendcounts);
    free(displ);
    free(my_u);
    free(my_u_prev);

    if (my_rank == 0) {
        printf("u_p[%d] = %lf\n", 45, u[45]);
    }

    return u;
}


int main(int argc, char *argv[]) {
    int N = 1000, my_rank, num_procs;
    double a = 0.2;
    double dt = 0.01;
    double final_T = 0.2;

    double *u_s;
    u_s = serial(N, a, dt, final_T);

    MPI_Init(&argc, &argv);
    //printf("u_s[%d] = %lf\n", 45, u_s[45]);

    double *u_p;
    u_p = parallel_mpi(N, a, dt, final_T, my_rank, num_procs);

    for (int i=0; i<N+2; i++) {
        if (u_s[i] != u_p[i]) {
            printf("Fail on element (%d): %lf != %lf\n", i, u_s[i], u_p[i]);
            return 1;
        }
    }

    MPI_Finalize();
    return 0;
}