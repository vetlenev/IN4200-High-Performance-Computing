#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Must supply a value for N");
        return 1;
    }
    else {
        int N = atoi(argv[1]);
    }

    int i, j, sqrt_N;

    char *array = malloc(N);
    array[0] = array[1] = 0;

    #pragma omp parallel {

    #pragma omp for
    for (i=2; i<N; i++) {
        array[i] = 1;
    }

    sqrt_N = (int)(sqrt(N));

    #pragma omp for nowait
    for (i=2; i<=sqrt_N; i++) {
        for (j=i*i; j<N; j+=1) {
            array[j] = 0;
        }
    }
    }

    free(array);
}

