#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int *block2D(int k, int M, int N, int P, int Q) {
    /* 
    2D block decomposition of M x N grid into
    P x Q blocks (i.e P rowwise blocks and Q colwise blocks)
    */
    int *bounds = malloc(4*sizeof(int));
    int x_start = (k/Q)*(M/P) + ((P - k/Q < M % P)?(M%P-(P-k/Q)):0);
    int x_stop = (k/Q + 1)*(M/P) + ((P - k/Q <= M % P)?(M%P-(P-k/Q)+1):0);
    int y_start = (k%Q)*(N/Q) + ((k%Q > (Q - N%Q))?(k%Q-(Q-N%Q)):0);
    int y_stop = (k%Q + 1)*(N/Q) + ((k%Q >= (Q - N%Q))?(k%Q-(Q-N%Q)+1):0);

    bounds[0] = x_start; bounds[1] = x_stop;
    bounds[2] = y_start; bounds[3] = y_stop;

    return bounds;
}

int main() {
    int M = 175, N = 230;
    int P = 8, Q = 9;

    int *start_stop = malloc(4*sizeof(int));
    start_stop = block2D(14, M, N, P, Q);
    printf("x_start: %d\n", start_stop[0]);
    printf("x_stop: %d\n", start_stop[1]);
    printf("y_start: %d\n", start_stop[2]);
    printf("y_stop: %d\n", start_stop[3]);
}