#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[]) {

    int rank, comm_size;
    MPI_Status status;
    char processorname[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    printf("comm_size: %d", comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processorname, &comm_size);
    char msg[100];
    sprintf(msg, "Hello world, I am rank nr. %d, processor %s, out of %d locality domains. ", rank, processorname, comm_size);
    int len_msg = strlen(msg)+1;

    if (rank == 0) {
        printf("Hello world, I am rank nr. %d, processor %s, out of %d locality domains\n", rank, processorname, comm_size); // print out its own message
        for (int i=1; i<comm_size; i++) {
            MPI_Recv(&msg, len_msg, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            puts(msg);
        }
    }
    else {
        MPI_Send(&msg, len_msg, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return 0;
}