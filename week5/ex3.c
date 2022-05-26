#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "week5.h"

#define CLOCK_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

void foo(int N, double **mat, double **s, int *v) { 
    int i,j; 
    double val; 
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) { 
            val = 1.0*(v[i] % 256); 
            mat[j][i] = s[j][i]*(sin(val)*sin(val)-cos(val)*cos(val)); 
        } 
    }
}

void foo_optimized(int N, double **mat, double **s, int *v) {
    int i,j; 
    double *val = (double*)malloc(N*sizeof(double));
    //double *sin_ = (double*)malloc(4*sizeof(double));
    //double *cos_ = (double*)malloc(4*sizeof(double));
    double *sin_cos = (double*)malloc(N*sizeof(double));   

    for (int i=0; i<N; i+=4) { // stride of 4
        
        val[i] = 1.0*(v[i] % 256);
        val[i+1] = 1.0*(v[i+1] % 256);
        val[i+2] = 1.0*(v[i+2] % 256);
        val[i+3] = 1.0*(v[i+3] % 256);

        sin_cos[i] = -cos(2*val[i]);
        sin_cos[i+1] = -cos(2*val[i+1]);
        sin_cos[i+2] = -cos(2*val[i+2]);
        sin_cos[i+3] = -cos(2*val[i+3]);

        for (j=0; j<N; j++) { 
            mat[j][i] = s[j][i]*sin_cos[i]; // Striding uses 4 times more of elements store in cache line
            mat[j][i+1] = s[j][i+1]*sin_cos[i+1];
            mat[j][i+2] = s[j][i+2]*sin_cos[i+2];
            mat[j][i+3] = s[j][i+3]*sin_cos[i+3];
        } 
    }
}

/* If foo is to be repeatedly called with different s and v, we can't rely on s and v to be a continuous part of cache.
Thus, we can't avoid cache misses and loads for s and v. 
Since we don't know size of s and v, it may be that only a minor part of them is actually used in calculation, 
in which case storing remaining elements in cache is bad cache utilization.

Optimizations:
- val not dependent on j => compute in outer loop
- sin and cos should be computed outside for-loop, using strided access
- compute each sin and cos term only once for each outer loop, and just use computed value to square them (speed efficient)
- If not possible to convert to row-major order, apply stride on outer loop to utilize more elements in cache line

- Access mat and s row-major simply by swapping their indices - this is allowed since both indices
loops through same length. BUT it requires val to be indexed by j instead of i, making it 
dependent on the inner loop. To alleviate memory usage, one can tabulate possible values of
val outside inner loop and just access these values (instead of computing them) inside inner loop.

*/

void foo_optimized_tab(int N, double **mat, double **s, int *v) {
    int i,j; 
    int *v_tab = (int*)malloc(N*sizeof(int));
    double *val_tab = (double*)malloc(256*sizeof(double));

    for (int i=0; i<256; i++) {
        val_tab[i] = -cos(2.0*i); // all possible values for val cronologically after index (accounts for negative arguments as well since cos(-x) = cos(x))
    }

    for (int i=0; i<N; i++) {
        v_tab[i] = v[i] % 256; // tabulate v to avoid having to do modulus-operation N^2 times
    }

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            // Indices of mat and s swapped -> val must now be calculated in inner loop - but this is optimized using tabulation
            mat[i][j] = s[i][j] * val_tab[v_tab[j]]; // val_tab and v_tab are not calculated in-place, they have been precomputed and the values are simply accessed by indexing the precomputed table
        } 
    }

    // Free tabulated arrays since these are only used locally inside function
    free(v_tab);
    free(val_tab); 
    // NB: If function is to be called multiple times, it is better to compute tabulated values 
    // outside the function, and instead pass the table as argument. In this case, one must not 
    // free the memory slots (except if table is NOT sent as pointer-argument, in which case
    // a local copy of table will be made).
}

int main() {

    int N=1000;
    int *v = (int*)malloc(N*sizeof(int));
    double **mat1, **mat2, **mat3, **s;

    calloc2D_point(&mat1, N, N); // Since mat stores result of matrix multiplication, it must be assigned to zero so that it starts accumulating from zero
    calloc2D_point(&mat2, N, N);
    calloc2D_point(&mat3, N, N);
    malloc2D_point(&s, N, N);

    // Assign values
    for (int i=0; i<N; i++) {
        v[i] = i;
        for (int j=0; j<N; j++) {
            s[i][j] = assignRand(-1.0, 1.0);
        }
    }


    clock_t timeA0 = clock();

    foo(N, mat1, s, v);

    clock_t timeA1 = clock();
    double tot_timeA = CLOCK_TO_MILLISEC((double)(timeA1 - timeA0));
    printf("Time foo unoptimized: %lfms\n", tot_timeA);

    clock_t timeB0 = clock();

    foo_optimized(N, mat2, s, v);

    clock_t timeB1 = clock();
    double tot_timeB = CLOCK_TO_MILLISEC((double)(timeB1 - timeB0));
    printf("Time foo sub-optimized: %lfms\n", tot_timeB);

    clock_t timeC0 = clock();

    foo_optimized_tab(N, mat3, s, v);

    clock_t timeC1 = clock();
    double tot_timeC = CLOCK_TO_MILLISEC((double)(timeC1 - timeC0));
    printf("Time foo full-optimized: %lfms\n", tot_timeC);

    return 0;
}