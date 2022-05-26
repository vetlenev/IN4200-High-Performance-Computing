#include <stdio.h>
#include <stdlib.h>

double get_rand(double rand, double min, double max) {
    double normalized = rand/(double)RAND_MAX;
    return min + normalized*(max - min);
}

double local_sum(double rand, int n, double min, double max) {
    // Sum up n random number. Done by one process.
    double psum = 0;
    for (size_t i=0; i<n; i++) {
        psum += get_rand(rand, min, max);
    }
    return psum;
}

double sum_locals(double *psums, int comm_size) {
    double tot_sum = 0;
    for (size_t i=1; i<comm_size; i++) {
        tot_sum += psums[i];
    }
    return tot_sum;
}

int printvec(double* y, int N){
    printf("[%.3lf", y[0]);
    for (size_t i = 1; i < N; i++) {
        printf(", %.3lf", y[i]);
    }
    printf("]\n");
    return 0;
}

int printmat(double* A, int N, int M){
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            printf("%7.2lf ", A[i*M + j]);
        }
        printf("\n");
    }
    return 0;
}