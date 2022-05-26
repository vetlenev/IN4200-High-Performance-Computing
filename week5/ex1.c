#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "week5.h"

#define CLOCK_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

// TASK A. Dense matrix-vector multiplication
int taskA() {
    double **A, *B, *y, *yy;
    int n=10000, m=10000, Lc = 5;
    //malloc2D_point(&A, n, m);
    A = malloc2D_nopoint(A, n, m);
    B = malloc1D_nopoint(B, m);
    y = malloc1D_nopoint(y, n); // standard
    yy = malloc1D_nopoint(yy, n); // unroll+jam


    for (int i=0; i<n; i++) {
        y[i] = 0.0;
        yy[i] = 0.0;
    }

    for (int j=0; j<m; j++) {
        B[j] = j*(m-j)*0.01;
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<m; j++) {
            A[i][j] = assignRand(0.0, 5.0);
        }
    }

    clock_t timeA0 = clock();
    // Matrix-vector multiplication, only multiplying lower triangular part of A
    for (int i=0; i<n; i++) {
        for (int j=0; j<=i; j++) {
            y[i] += A[i][j]*B[j];
        }
    }

    clock_t timeA1 = clock();
    double tot_timeA = CLOCK_TO_MILLISEC((double)(timeA1 - timeA0));
    printf("Time unoptimized: %lfms\n", tot_timeA);

    clock_t timeB0 = clock();

    int i, j;
    int rest = n%4;
    for (i=0; i<n-rest; i+=4) {
        for (j=0; j<=i; j++) {
            yy[i] += A[i][j]*B[j];
            yy[i+1] += A[i+1][j]*B[j];
            yy[i+2] += A[i+2][j]*B[j];
            yy[i+3] += A[i+3][j]*B[j];
        }
        // Since inner loop depends on outer (strided) loop, the inner loop will jump over some j-values associated with each i-value
        // Since inner loop stops at j=i, computing strided value yy[i+k] in inner loop will miss out on k values (namely those from i+1 to i+k)
        yy[i+1] += A[i+1][j+1]*B[j+1];
        yy[i+2] += A[i+2][j+1]*B[j+1];
        yy[i+2] += A[i+2][j+2]*B[j+2];
        yy[i+3] += A[i+3][j+1]*B[j+1];
        yy[i+3] += A[i+3][j+2]*B[j+2];
        yy[i+3] += A[i+3][j+3]*B[j+3];
    }
    // Remainder loop
    for (int ir=n-rest; ir<n; ir++) {
        for (int j=0; j<=ir; j++) {
            yy[ir] += A[ir][j]*B[j];
        }
    }

    clock_t timeB1 = clock();
    double tot_timeB = CLOCK_TO_MILLISEC((double)(timeB1 - timeB0));
    printf("Time unroll+jam: %lfms\n", tot_timeB);

    int flops = 2*n*m;
    double data_traffic = n*m/Lc + m/Lc + n/Lc;
    
    double code_balance = data_traffic / flops;
    printf("Code balance - dense matrix-vector multiply: %lf\n", code_balance);

    free(y);
    free(yy);
    free(B);
    free(A);
    free(A[0]); //only need to free first array element, since the contiguous array builds upon this memory location

    return 0;
}

int upperTriangular() {
    double **A, *B, *y;
    int n=4, m=4;
    y = calloc1D_nopoint(y, n); // Nopoint makes a local copy of array, therefore need to return array in function
    malloc2D_point(&A, n, m); // allocating using pointers works just as fine
    B = malloc1D_nopoint(B, m);

    for (int j=0; j<m; j++) {
        B[j] = j;
    }

    for (int i=0; i<n; i++) {
        for (int j=0; j<m; j++) {
            A[i][j] = i+j;
        }
    }

    int i, j;
    int rest = n%4;
    for (i=0; i<n-rest; i+=4) {
        for (j=i; j<m; j++) {
            y[i] += A[i][j]*B[j];
            y[i+1] += A[i+1][j]*B[j];
            y[i+2] += A[i+2][j]*B[j];
            y[i+3] += A[i+3][j]*B[j];
        }
        // Since inner loop starts from j=i, the updates for y[i+k] are invalid since they 
        // include k too many summations => these must be removed by postprocessing
        y[i+1] -= A[i+1][i]*B[i];
        y[i+2] -= A[i+2][i]*B[i];
        y[i+2] -= A[i+2][i+1]*B[i+1];
        y[i+3] -= A[i+3][i]*B[i];
        y[i+3] -= A[i+3][i+1]*B[i+1];
        y[i+3] -= A[i+3][i+2]*B[i+2];
    }
    // Remainder loop
    for (int i=n-rest; i<n; i++) {
        for (int j=i; j<m; j++) {
            y[i] = A[i][j]*B[j];
        }
    }

    // Print results
    for (int i=0; i<n; i++) {
        for (int j=0; j<m; j++) {
            printf("A[%d][%d] = %.1lf\n", i, j, A[i][j]);
        }
    }
    printf("\n");
    for (int j=0; j<m; j++) {
        printf("B[%d] = %.1lf\n", j, B[j]);
    }
    printf("\n");
    for (int i=0; i<n; i++) {
        printf("y[%d] = %.1lf\n", i, y[i]);
    }
}

int main() {
    //taskA();
    upperTriangular();
    return 0;
}