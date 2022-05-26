#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define _USE_MATH_DEFINES

double **wave_serial(int M, int N, double C, double dt, double T) {
    double **up = malloc((M+2)*sizeof *up);
    double **u = malloc((M+2)*sizeof *u);
    double **um = malloc((M+2)*sizeof *um);
    up[0] = malloc((M+2)*(N+2)*sizeof up);
    u[0] = malloc((M+2)*(N+2)*sizeof u);
    um[0] = malloc((M+2)*(N+2)*sizeof um);
    double **tmp_ptr;

    for (int i=1; i<(M+2); i++) {
        up[i] = &up[0][i*(N+2)];
        u[i] = &u[0][i*(N+2)];
        um[i] = &um[0][i*(N+2)];
    }

    for (int i=0; i<(M+2); i++) {
        for (int j=0; j<(N+2); j++) {
            um[i][j] = 0.0;
            u[i][j] = sin(M_PI*i/(M+1)) + sin(M_PI*j/(N+1));
        }
    }
    printf("u_s[%d][%d] = %lf\n", 1, 3, u[1][3]);
    printf("u_s_start[%d][%d] = %lf\n", 45, 37, u[45][37]);

    double t = 0.0;
    while (t < T) {
        for (int i=1; i<(M+1); i++) {
            for (int j=1; j<(N+1); j++) {
                up[i][j] = 2*u[i][j] - um[i][j] + C*(u[i-1][j] - 2*u[i][j] + u[i+1][j])
                                    + C*(u[i][j-1] - 2*u[i][j] + u[i][j+1]);
            }
        }

        for (int ii=0; ii<(M+2); ii++) {
            up[ii][0] = sin(M_PI*ii/(M+1));
            up[ii][N+1] = sin(M_PI*ii/(M+1));
        }
        for (int jj=0; jj<(N+2); jj++) {
            up[0][jj] = sin(M_PI*jj/(N+1));
            up[M+1][jj] = sin(M_PI*jj/(N+1));
        }

        t += dt;

        tmp_ptr = um;
        //tmp_ptr_up = up;
        um = u;
        u = up;
        up = tmp_ptr;
    }

    printf("u_s[%d][%d] = %lf\n", 45, 37, u[45][37]);

    return u;
}

double **wave_mpi(int M, int N, double C, double dt, double T, int my_rank, int num_procs) {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double **global_up;
    double **global_u;
    double **global_um;
    double t = 0.0;

    if (my_rank == 0) {
        global_up = malloc((M+2)*sizeof *global_up);
        global_u = malloc((M+2)*sizeof *global_u);
        global_um = malloc((M+2)*sizeof *global_um);
        global_up[0] = malloc((M+2)*(N+2)*sizeof global_up);
        global_u[0] = malloc((M+2)*(N+2)*sizeof global_u);
        global_um[0] = malloc((M+2)*(N+2)*sizeof global_um);

        for (int i=1; i<(M+2); i++) {
            global_up[i] = &global_up[0][i*(N+2)];
            global_u[i] = &global_u[0][i*(N+2)];
            global_um[i] = &global_um[0][i*(N+2)];
        }

        for (int i=0; i<(M+2); i++) {
            for (int j=0; j<(N+2); j++) {
                global_u[i][j] = sin(M_PI*i/(M+1)) + sin(M_PI*j/(N+1));
                global_um[i][j] = 0.0;
            }
        }

        printf("u_p_start[%d][%d] = %lf\n", 1, 3, global_u[1][3]);
    }

    double **up;
    double **u;
    double **um;
    double **tmp_ptr;

    int *sendcounts = malloc(num_procs*sizeof sendcounts);
    int *n_rows = malloc(num_procs*sizeof n_rows);
    int *displ = malloc(num_procs*sizeof displ); // scattering and gathering is done for same array -> only need one displ
    displ[0] = 0;
    for (int rank=0; rank<num_procs-1; rank++) {
        n_rows[rank] = (M/num_procs) + ((rank < (M%num_procs))?1:0);
        sendcounts[rank] = n_rows[rank]*(N+2); // assign all columns
        displ[rank+1] = displ[rank] + sendcounts[rank];
    }
    n_rows[num_procs-1] = (M/num_procs);
    sendcounts[num_procs-1] = n_rows[num_procs-1]*(N+2);

    int my_M = n_rows[my_rank]; // ranks have different nr rows, same nr cols
    int my_M_start = my_rank*M/num_procs;
    int my_M_stop = (my_rank+1)*M/num_procs;
    int my_count = sendcounts[my_rank];

    up = malloc((my_M+2)*sizeof *up); // remember top+bottom ghosts
    u = malloc((my_M+2)*sizeof *u);
    um = malloc((my_M+2)*sizeof *um);
    
    up[0] = malloc((my_M+2)*(N+2)*sizeof up);
    u[0] = malloc((my_M+2)*(N+2)*sizeof u);
    um[0] = malloc((my_M+2)*(N+2)*sizeof um);

    for (int i=1; i<(my_M+2); i++) {
        up[i] = &up[0][i*(N+2)];
        u[i] = &u[0][i*(N+2)];
        um[i] = &um[0][i*(N+2)];
    }

    MPI_Scatterv(global_up + (N+2), // omit first boundary row
                sendcounts,
                displ,
                MPI_DOUBLE,
                up + (N+2),
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    MPI_Scatterv(global_u + (N+2), // omit first boundary row
                sendcounts,
                displ,
                MPI_DOUBLE,
                u + (N+2),
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    MPI_Scatterv(global_um + (N+2), // omit first boundary row
                sendcounts,
                displ,
                MPI_DOUBLE,
                um + (N+2),
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    // Fix boundary columns
    /*for (int i=1; i<(my_M+1); i++) {
        u[i][0] = sin(M_PI*(i+my_M_start)/(M+1));
        um[i][0] = 0.0;
        u[i][N+1] = sin(M_PI*(i+my_M_start)/(M+1));
        um[i][N+1] = 0.0;
    }*/

    if (my_rank == 0) { // Fix top (global) boudary row
        for (int j=0; j<(N+2); j++) {
            u[0][j] = sin(M_PI*j/(N+1));
            um[0][j] = 0.0;
        }
    }
    if (my_rank == num_procs-1) { // Fix bottom (global) boundary row
        for (int j=0; j<(N+2); j++) {
            u[my_M+1][j] = sin(M_PI*j/(N+1));
            um[my_M+1][j] = 0.0;
        }
    }

    tmp_ptr = malloc((my_count+2*N)*sizeof *tmp_ptr);

    if (my_rank == 0) {
        printf("u_p[%d][%d] = %lf\n", 1, 3, u[1][3]);
    }
    printf("u_test: %lf\n", u[1][3]);

    while (t < T) {
        // Communicate ghost points BEFORE computations
        // Otherwise:
        //      First iteration: Ghost point is nonsense
        //      Later iterations: Ghost points lag one step behind
        MPI_Request request_from_top;
        MPI_Request request_from_bottom;
        MPI_Status status_top;
        MPI_Status status_bottom;
        double top_ghost[N+2]; // initialized just as emtpy address - to be filled when neighbor value is received
        double *top_boundary = malloc((N+2)*sizeof top_boundary);
        double *bottom_boundary = malloc((N+2)*sizeof bottom_boundary);
        double bottom_ghost[N+2];
        for (int j=0; j<(N+2); j++) {
            top_boundary[j] = u[1][j];
            bottom_boundary[j] = u[my_M][j];
        }
        
        if (my_rank % 2 != 0) { // odd ranks send first
            if (my_rank != num_procs-1)
                MPI_Send(&bottom_boundary, N+2, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD);

            MPI_Send(&top_boundary, N+2, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD); // odd nodes can always send top

            if (my_rank != num_procs-1) {
                MPI_Irecv(&bottom_ghost, N+2, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, &request_from_bottom);
                MPI_Wait(&request_from_bottom, &status_bottom);
                for (int j=0; j<(N+2); j++)
                    u[my_M+1][j] = bottom_ghost[j];
            }

            MPI_Irecv(&top_ghost, N+2, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &request_from_top);
            MPI_Wait(&request_from_top, &status_top);
            for (int j=0; j<(N+2); j++)
                u[0][j] = top_ghost[j];
        }
        else {
            if (my_rank != num_procs-1) {
                MPI_Irecv(&bottom_ghost, N+2, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD, &request_from_bottom);
            }
            if (my_rank != 0) {
                MPI_Irecv(&top_ghost, N+2, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD, &request_from_top);
            } // Wait necessary to ensure communication is finished before doing computations

            if (my_rank != num_procs-1) {
                MPI_Send(&bottom_boundary, N+2, MPI_DOUBLE, my_rank+1, 0, MPI_COMM_WORLD);
            }
            if (my_rank != 0) {
                MPI_Send(&top_boundary, N+2, MPI_DOUBLE, my_rank-1, 0, MPI_COMM_WORLD);
            }
            if (my_rank != num_procs-1) {
                MPI_Wait(&request_from_bottom, &status_bottom);
               for (int j=0; j<(N+2); j++)
                    u[my_M+1][j] = bottom_ghost[j];
            }
            if (my_rank != 0) {
                MPI_Wait(&request_from_top, &status_top);
                for (int j=0; j<(N+2); j++)
                    u[0][j] = top_ghost[j];
            }
        }

        // Interior computations
        for (int i=1; i<(my_M+1); i++) {
            for (int j=1; j<(N+1); j++) {
                up[i][j] = 2*u[i][j] - um[i][j] + C*(u[i-1][j] - 2*u[i][j] + u[i+1][j])
                                    + C*(u[i][j-1] - 2*u[i][j] + u[i][j+1]);
            }
        }

        // Fix boundary columns
        for (int ii=0; ii<(my_M+2); ii++) {
            up[ii][0] = sin(M_PI*(ii+my_M_start)/(M+1));
            up[ii][N+1] = sin(M_PI*(ii+my_M_start)/(M+1));
        }
        if (my_rank == 0) { // Fix global top row
            for (int jj=0; jj<(N+2); jj++)
                up[0][jj] = sin(M_PI*jj/(N+1));
        }
        if (my_rank == num_procs-1) { // Fix global bottom row
            for (int jj=0; jj<(N+2); jj++)
                up[my_M+1][jj] = sin(M_PI*jj/(N+1));
        }

        t += dt;

        tmp_ptr = um;
        um = u;
        u = up;
        up = tmp_ptr;
    }

    MPI_Gatherv(u + (N+2),
                sendcounts[my_rank],
                MPI_DOUBLE,
                global_u + (N+2), // don't account for top and bottom row
                sendcounts,
                displ,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    if (my_rank == 0) 
        printf("u_p[%d][%d] = %lf\n", 45, 37, global_u[45][37]);

    free(sendcounts);
    free(displ);
    free(u);
    free(um);
    free(up);

    MPI_Finalize();
    return global_u;
}

int main(int argc, char *argv[]) {
    int M = 80;
    int N = 100;
    double C = 0.2;
    double dt = 0.01;
    double T = 1.0;

    double **u_s;
    u_s = wave_serial(M, N, C, dt, T);

    MPI_Init(&argc, &argv);
    double **u_p;
    int my_rank, num_procs;
    u_p = wave_mpi(M, N, C, dt, T, my_rank, num_procs);

    return 0;
}