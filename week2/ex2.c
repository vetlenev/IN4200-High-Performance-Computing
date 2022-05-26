#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define CLOCKS_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

int *hand_coded(int *dest, int *src, int n) {
    for (int i=0; i<n; i++) {
        dest[i] = src[i];
    }
    return dest;
}

//int *memcpy(int *dest, int *src, int n);

int main() {
    clock_t c_hand_start, c_hand_stop, c_mem_start, c_mem_stop;
    int n=1000000;
    int *src = (int*)malloc(n*sizeof(int));
    int *dest = (int*)malloc(n*sizeof(int));

    for (int i=0; i<n; i++) {
        src[i] = i;
    }

    //printf("Before copy: %d\n", dest[1]);
    c_hand_start = clock();
    hand_coded(dest, src, n);
    c_hand_stop = clock() - c_hand_start;
    //printf("After copy: %d\n", dest[1]);

    c_mem_start = clock();
    memcpy(dest, src, n);
    c_mem_stop = clock() - c_mem_start;

    printf("Time own: %lu\n", CLOCKS_TO_MILLISEC(c_hand_stop));
    printf("Time memcpy: %lu\n", CLOCKS_TO_MILLISEC(c_mem_stop));
    // memcpy is significantly faster than copying array items manually.
    // perhaps memcpy vectorizes the instruction, so large chunks of the array are copied simultaneously
    free(src); // Necessary to free array since it is created DYNAMICALLY using malloc
    free(dest);
    // Only need to free the array itself not the memory, because it's a 1D array where only the array itself has been malloced
    // But if we have 2D array where we also malloc its inner arrays, the inner arrays must be free'd.
}