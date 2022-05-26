#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int parallel_count_occurence(const char *text_string, const char *pattern, int my_rank, int num_procs) {
    int n, p;
    if (my_rank == 0) {
        n = strlen(text_string);
        p = strlen(pattern);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD); // Stop here to ensure all processes have got value of p
    MPI_Bcast(pattern, p+1, MPI_CHAR, 0, MPI_COMM_WORLD);

    int my_n = n / num_procs; // Assume num_procs is a global variable accessible for all functions
    int remainder_n = n % num_procs;

    if (my_rank < remainder_n) 
        my_n += 1;

    if (my_rank == num_procs - 1)
        my_n += 1; // to include end character

    if (my_n < p) { // Make sure substrings are not shorter than pattern
        printf("Processor %d has received string of length %d less than pattern length %d\n", my_rank, my_n, p);
        return 1;
    }

    printf("Rank %d, my_n: %d\n", my_rank, my_n);

    char my_text[my_n]; // Substring assigned to each process

    int *sendcounts = malloc(num_procs*sizeof(int));
    int *displacement = malloc(num_procs*sizeof(int));
    displacement[0] = 0;
    int base_str_len = n / num_procs;

    for (int rank=0; rank<num_procs; rank++) {
        if (rank < remainder_n)
            sendcounts[rank] = base_str_len + 1;
        else
            sendcounts[rank] = base_str_len;

        if (rank < num_procs-1)
            displacement[rank+1] = displacement[rank] + sendcounts[rank];
    }

    MPI_Scatterv(text_string,
                 sendcounts,
                 displacement,
                 MPI_CHAR,
                 my_text,
                 sendcounts[my_rank],
                 MPI_CHAR,
                 0,
                 MPI_COMM_WORLD);

    //printf("Rank %d, my_text: %s\n", my_rank, my_text);

    int num_matches = 0;
    int result = 0; // to hold result, only significant at root

    for (int k=0; k<my_n-p+1; k++) {
        char text_sub[p]; // substring of substring
        for (int i=k; i<k+p; i++) {
            text_sub[i-k] = my_text[i];
        }
        int match = !strncmp(text_sub, pattern, p);
        num_matches += match;
    }
    printf("Rank %d, num_matches: %d\n", my_rank, num_matches);

    /*
    NB: The pattern might overlap between processes, in will not yield a match.
    To solve this, the values at the boundary must be communicated if they end/start with the pattern.
    */
    /*if (my_rank % 2 != 0) {
        if (string_match(sub_pattern, start_string) || string_match(sub_pattern, end_string))
            MPI_Send();
    }*/

    // Because we are summing results instead of sending arrays, we can simplify by using Reduce instead of Gatherv
    MPI_Reduce(&num_matches, &result, 1, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);

    return result;
}   


int main(int argc, char *argv[]) {

    int my_rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //int n = 24; // length of string
    //int p = 4; // length of pattern
    const char *text_string;
    const char *pattern;

    if (my_rank == 0) {
        text_string = "RABCABCGHSABCGABCABCODMABCILJABC";
        pattern = "ABC";
    }

    int num = parallel_count_occurence(text_string, pattern, my_rank, num_procs);

    if (my_rank == 0)
        printf("Number of occurences: %d\n", num);

    MPI_Finalize();
}
