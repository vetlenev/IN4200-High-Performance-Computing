#include <stdio.h>
#include <stdlib.h>

void divide(int n, int P, int k, int *start_k, int *stop_k) {
    int base_k = n / P;
    int rem = n % P;
    int rem_rank_start = k - (P-rem) + 1;

    *start_k = base_k*k + ((rem_rank_start>1)?(rem_rank_start-1):(0));
    *stop_k = base_k*(k+1)-1 + ((rem_rank_start>0)?(rem_rank_start):(0));
    
    printf("Start: %d\n", *start_k);
    printf("Stop: %d\n", *stop_k);
}

int main() {
    int n = 14;
    int P = 4;
    int k = 3;
    int start_k, stop_k;

    divide(n, P, k, &start_k, &stop_k);
}