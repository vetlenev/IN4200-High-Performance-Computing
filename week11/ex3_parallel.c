#include <malloc.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    int rank, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int M_full, M_rank, T; // Must be declared for all ranks, but only one rank needs to initialize them
    if (rank == 0) {
        if (argc < 2) {
            M_full = 42;
        } else {
            M_full = atoi(argv[1]);
        }
        if (argc < 3) {
            T = 42;
        } else {
            T = atoi(argv[2]);
        }
    }

    MPI_Bcast(&M_full, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);


    int remainder = M_full % np;
    int base_chunk = M_full / np;

    int *nr_locals = malloc(np*sizeof(int)); // nr nodes assigned to given rank
    int *displacement = malloc(np*sizeof(int)); // indices for start and stop of local assigned nodes
    displacement[0] = 0;
    for (int i=1; i<=np; i++) {
        int i_start_next = i*base_chunk;
        if (i < size) {
            displacement[i] = i_start_next;
        }
        nr_locals[i-1] = i_start_next - displacement[i-1]
    }

    M_rank = nr_locals[rank];

    double x, dx = 1.0/(M_full+1);
    double t, dt = 1. / (T-1);
    double *um, *u, *up;
    int i;

    // Initial conditions
    if (rank == 0) {
        um = malloc((M_full+2)*sizeof(double)); // +2 accounts for ghost points
        u = malloc((M_full+2)*sizeof(double));
        up = malloc((M_full+2)*sizeof(double));

        for (i=0; i<M_full+2; i++) {
            x = i*dx;
            um[i] = x > .5 ? 1. : -1.; // Initial values
        }

        for (i=1; i<M_full+1; i++) // Update initial values with centered difference
            u[i] = um[i] + 0.5*(um[i-1]-2*um[i]+um[i+1]);

        u[0] = u[M_full+1] = 0.0;
    }

    // Each rank is assigned M interior points and 2 ghost (or boundary) points
    double *um_rank = malloc((M+2)*sizeof(double));
    double *u_rank = malloc((M+2)*sizeof(double));
    double *up_rank = malloc((M+2)*sizeof(double));
    double *tmp_rank;

    // Use rank 0 to scatter initial values to associated rank
    MPI_Scatterv(um+1, nr_locals, displacement, MPI_DOUBLE, um_rank+1, M_rank, MPI_DOUBLE, 0, MPI_COMM_WORLD); // +1 to skip first array location, since this belongs to fixed boundary point
    MPI_Scatterv(u+1, nr_locals, displacement, MPI_DOUBLE, u_rank+1, M_rank, MPI_DOUBLE, 0, MPI_COMM_WORLD); // +1 does not add 1 to array, it displaces address position one space

    if (rank == 0) { // Set leftmost boundary value
        um_rank[0] = 0.0; // Remember: index 0 for rank 0 is this rank's left ghost point, but in this case it is the true left boundary value
        u_rank[0] = 0.0;
        up_rank[0] = 0.0;
    }

    if (rank == np - 1) {
        um_rank[M+1] = 0.0; // Remember: last index for last rank is this rank's right ghost point, but in this case it is the true right boundary value
        u_rank[M+1] = 0.0;
        up_rank[M+1] = 0.0;
    }

    t = dt;
    while (t<1.0) {
        t += dt;
        double left_buff, right_buff;
        double left_ghost, right_ghost;
        MPI_Status status;

        // To avoid deadlocks, we let odd-numbered ranks send first
        if (rank % 2 == 0) { // Only send u, since um has been sent in previous iteration
            if (rank > 0) { // Can't send left true boundary value to non-existing left neighbor
                left_buff = u_rank[1]; // index 1 is INNER left boundary value to be sent to left neighbor. NB: not ghost point!
                MPI_Send(&left_buff, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
            }
            if (rank < np-1) { // Can't send right tru boundary value to non-existing right neighbor
                right_buff = u_rank[M];
                MPI_Send(&right_buff, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            }
            // Odd ranks send first, but they also need to receive
            if (rank < np-1) {
                MPI_Recv(&left_buff, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &status);
            }
            if (rank > 0) {
                MPI_Recv(&right_buff, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status)
            }
        }

        else {
            if (rank < np-1) { // Can't receive boundary value from non-existing left neighbor
                //left_ghost = u[0];
                MPI_Recv(&left_buff, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &status); // Receive from right neighbor
            }
            if (rank > 0) {
                MPI_Recv(&right_buff, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status);
                left_buff = u_rank[1];
                MPI_Send(&left_buff, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
            }
            if (rank < np-1) {
                right_buff = u_rank[M];
                MPI_Send(&right_buff, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            }
        }
        // Inner loop is common for all ranks
        for (i=1; i<=M; i++)
            up_rank[i] = (um_rank[i]+u_rank[i-1]+u_rank[i+1]) / 3;

        tmp_rank = um_rank;
        um_rank = u_rank;
        u_rank = up_rank; // Why doesn't um get overwritten here? Why don't we need two temporary arrays?
        up_rank = tmp_rank;

    }

    // After computations, gather local results to rank 0 to assemble results. Since computations are done, only u need to be sent.
    MPI_Gatherv(u_rank+1, M, MPI_DOUBLE, u+1, nr_locals, displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Boundary points already computed - don't need to be sent -> offset 1 to skip this
    // Once local results are gathered, they can be erased
    free(um_rank);
    free(u_rank);
    free(up_rank);

    // Output for plotting
    FILE *fp = fopen("w11_e3.dat", "w");
    for (size_t i = 0; i < M + 2; i++) {
        fprintf(fp, "%.17g, %.17g\n", (double)i/(M+1),
                                      up[i]);
    }
    fclose(fp);

    // After writing result to file, global arrays can be freed
    if (rank == 0) {
        free(um);
        free(u);
    }

    MPI_Finalize();
}
