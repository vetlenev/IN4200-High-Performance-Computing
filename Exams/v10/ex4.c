#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double Euclidean_norm (double* a, int n)
{
    double sum = 0.;
    int i;
    for (i=0; i<n; i++)
        sum += a[i]*a[i];

    return sqrt(sum);
}

double Euclidean_norm_MPI (double* a, int n)
{
    double sum = 0.;
    int i;
    for (i=0; i<n; i++)
        sum += a[i]*a[i];

    return sum;
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("Need more input arguments.\n");
        return 1;
    }

    int my_rank, num_procs;
    int n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    // Assume n very large, so elements of a are distributed among processors
    // Each computation is independent of each other => no need for communication

    if (my_rank == 0)
        n = atoi(argv[1]);
    
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    double *a = malloc(n*sizeof a);
    if (my_rank == 0) {
        for (int i=0; i<n; i++) {
            a[i] = i%5 + 0.001*i;
        }
    }
    
    // Divide work equally among processors
    int base = n / num_procs;
    int rem = n % num_procs;
    printf("Rem: %d\n", rem);

    int *sendcounts = malloc(num_procs*sizeof sendcounts);
    int *displ = malloc(num_procs*sizeof displ);
    displ[0] = 0;
    for (int rank=0; rank<num_procs-1; rank++) {
        sendcounts[rank] = base + ((rank<rem)?1:0);
        displ[rank+1] = displ[rank] + sendcounts[rank];
    }
    sendcounts[num_procs-1] = base + ((num_procs-1<rem)?1:0);

    double *my_a = malloc(sendcounts[my_rank]*sizeof my_a);

    MPI_Scatterv(a,
                sendcounts,
                displ,
                MPI_DOUBLE,
                my_a,
                sendcounts[my_rank],
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    double part_sum = Euclidean_norm_MPI(my_a, sendcounts[my_rank]);
    double tot_sum;

    MPI_Reduce(&part_sum, &tot_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        tot_sum = sqrt(tot_sum);
        printf("Result from reduction: %lf\n", tot_sum);

        double serial_result = Euclidean_norm(a, n);
        printf("Serial result: %lf\n", serial_result);
    }

    free(my_a);
    if (my_rank == 0)
        free(a);

    MPI_Finalize();
    return 0;
}