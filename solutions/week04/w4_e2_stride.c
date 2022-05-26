// Testing the memory bandwidth.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "week4.h" // This header we defined!

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Usage: ./prog.exe N_ELEMENTS STRIDE\n");
        return 1;
    }
    int N = atoi(argv[1]);
    int stride = atoi(argv[2]);

    double *A = malloc(N * sizeof *A);
    double *B = malloc(N * sizeof *B);

    // Assigning some random numbers:
    srand(time(NULL)); // Seeds the random number generator.

    for (size_t i = 0; i < N; i++) {
        A[i] = randfrom(0., 2.); // This function is from our header.
        B[i] = randfrom(0., 2.);
    }

    double s = 1.5;

    clock_t start = clock();
    for (size_t i = 0; i < N; i+=stride) {
        A[i] = s*B[i];
    }
    clock_t end = clock();

    double total = (double)(end - start)/CLOCKS_PER_SEC;
    int n_bytes = 2 * (int)(N/(double)stride + 0.5) * sizeof *A;
    int flops = (int)(N/(double)stride + 0.5);

    printf("Time: %lfms\n", 1000*total);
    printf("Bytes per sec: %3e\n", n_bytes/total);
    printf("Flops: %3e\n", flops/total);

    free(A);
    free(B);

    return 0;
}
