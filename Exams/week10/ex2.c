#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double option1(int N) {
    int my_rank, num_procs;
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    MPI_Request request[num_procs-1];
    // Even distribution of work
    int myN = N/num_procs + ((my_rank < N%num_procs)?1:0);
    //double rand_nums = malloc(myN*sizeof rand_nums);
    // Partial sums
    double psum = 0;
    for (int i=0; i<myN; i++) {
        psum += randfrom(0.0, 1.0);
    }
    // Initialize receives
    double *recv = malloc((num_procs-1)*sizeof recv);
    double tot_sum;
    
    if (my_rank == 0) {
        // initialize Irecv to make non-blocking communication happen while computation is done
        for (int i=1; i<num_procs; i++)
            MPI_Irecv(&recv[i-1], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &request[i-1]);
    }

    if (my_rank == 0) {
        tot_sum = psum;
        MPI_Waitall(num_procs-1, request, MPI_STATUSES_IGNORE);
        
        for (int j=0; j<num_procs-1; j++) {
            tot_sum += recv[j];
        }

        printf("Option 1 sum: %lf\n", tot_sum);
    }
    else {
        MPI_Send(&psum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    return tot_sum;
}

double option2(int N) {
    int my_rank, num_procs;
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    double tot_sum;

    int myN = N/num_procs + ((my_rank < N%num_procs)?1:0);

    double psum = 0;
    for (int i=0; i<myN; i++) {
        psum += randfrom(0.0, 1.0);
    }

    double all_sums[num_procs-1];

    MPI_Gather(&psum,
                1,
                MPI_DOUBLE,
                all_sums,
                1,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    if (my_rank == 0) {
        tot_sum = psum;
        for (int i=0; i<num_procs-1; i++) {
            tot_sum += all_sums[i];
        }
        printf("Option 2 sum: %lf\n", tot_sum);
    }

    return tot_sum;
}

double option3(int N) {
    int my_rank, num_procs;
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    double tot_sum;

    int myN = N/num_procs + ((my_rank < N%num_procs)?1:0);

    double psum = 0;
    for (int i=0; i<myN; i++) {
        psum += randfrom(0.0, 1.0);
    }

    MPI_Reduce(&psum, &tot_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Option 3 sum: %lf\n", tot_sum);
    }

    return tot_sum;
}

int main(int argc, char* argv[]) {
    int N = 1000;
    double sum1, sum2, sum3;

    MPI_Init(&argc, &argv);
    sum1 = option1(N);
    sum2 = option2(N);
    sum3 = option3(N);

    MPI_Finalize();

    return 0;
}