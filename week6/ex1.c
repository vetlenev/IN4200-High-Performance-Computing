#include <stdio.h>
#include <stdlib.h>

int main() {
    // n = tasksize, p = nr workers, r = remainder (n % p)
    int n = 16; p = 6;
    int r = n % p;

    int w = n \ p; // tasks per worker
    double *W; // Array of workers

    // Assign w tasks to each worker ...

    for (int i=0; i<r; i++) {
        W[i] = 1; // Assign one additional task to r first workers
        // Assigning to random worker is more costly, since we may need to jump in cache line representing the workers
    }

}