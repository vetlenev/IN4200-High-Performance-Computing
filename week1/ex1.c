#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double computeSum(void) { // return type of function is float (double)
    double sum = 0;
    double i = 0, minus = -1;

    for (i=0; i<10; i++) { 
        int sign = pow(minus, i); // first factor is int
        double partSum = 1/pow(2,2*i); // second factor is float
    
        sum += sign * partSum; // C is able to multiply int and float
    }
    return sum;
}

void allocateArray(int n, double *min, double *max) {
    double *A = (double*)malloc(n*sizeof(double));
    int i=0;

    for (i=0; i<n; i++) {
        A[i] = rand();
    }

    for (i=0; i<n; i++) {
        if (A[i] < *min) {
            *min = A[i];
        }

        if (A[i] > *max) {
            *max = A[i];
        }
    }
}

double** traverseRows(int m, int n) {
    double **A = (double**)malloc(m*sizeof(double*));
    int i=0, j=0;

    // Traverse rows first, then columns
    for (i=0; i<m; i++) {
        A[i] = (double*)malloc(n*sizeof(double));
        for (j=0; j<n; j++) {
            A[i][j] = rand();
        }
    }

    return A;
}

double** traverseColumns(int m, int n) {
    double** A = (double**)malloc(n*sizeof(double*));
    int i=0, j=0;

    // Traverse columns first, then rows
    for (j=0; j<n; j++) {
        A[j] = (double*)malloc(m*sizeof(double));
        for (i=0; i<m; i++) {
            A[j][i] = rand();
        }
    }

    return A;
}

void array3D(int nx, int ny, int nz) {
    //double* u_store1D = (double*)malloc(nx*ny*nz*sizeof(double)); // Flattened 1D contiguous data storage
    //double** u_store2D = (double**)malloc(nx*ny*sizeof(double*)); // 2D contiguous data storage
    double*** u = (double***)malloc(nx*sizeof(double**)); // Default 3D storage - nx rows
    u[0] = (double**)malloc(nx*ny*sizeof(double*)); // 2D contiguous storage - nx rows, ny cols
    u[0][0] = (double*)malloc(nx*ny*nz*sizeof(double)); // 1D contiguous storage - nx rows, ny cols, nz cells
    int i, j, k;
    
    for (i=1; i<nx; i++) {
        u[i] = &(u[0][i*ny]);
    }

    for (j=1; j<nx*ny; j++) {
        u[0][j] = &(u[0][0][j*nz]); // storage for last array element in u should be allocated the total number of elements (j=nx*ny*nz)
    }

    // Allocate:
    for (int i=0; i<nx; i++) {
        for (int j=0; j<ny; j++) {
            for (int k=0; k<nz; k++) {
                u[i][j][k] = i*ny*nz + j*nz + k;
                //printf("u[%d][%d][%d] = %d\n", i, j, k, (int)u[i][j][k]); // Need to convert double to int since the assigned values are of type int
            }
        }
    }

    printf("u: %d\n", u);
    printf("u[0]: %d\n", &u[0]); // points to first row of array (basically array start) and sets of nx*ny*size_of(double*)=4*3*? positions in memory. 
    printf("u[0][0]: %d\n", &u[0][0]); // 24=ny*size(double) elements transversed from u[0] before we reach address of u[0][0]
    printf("u[0][0][0]: %d\n", &u[0][0][0]);
    printf("u[4][3][2]: %d\n", &u[3][2][1]); // nx*ny*nz*size(double) = 4*3*2*8=192 elements between start u[0][0][0] and end u[3][2][1]+8 of array

    // Deallocate memory 
    free(u[0][0]);
    free(u[0]);
    free(u);
    // Need to deallocate all inner arrays, in addition to the outer, since all three have been
    // manually allocated specific memory addresses
}

int main(void) {
    double sum = computeSum();
    printf("Limit of sum: %.4lf\n", sum); // print with 4 decimal numbers

    double min=(double)RAND_MAX, max=0;
    allocateArray(10, &min, &max);
    printf("Min of allocated array: %lf\n", min);
    printf("Max of allocated array: %lf\n", max);

    double** array2D = traverseRows(3,2);
    //double** array2D = traverseColumns(3,2); 
    printf("Size of array: %d\n", sizeof(array2D));
    printf("Array element (1,0): %lf\n", array2D[1][0]);
    // Traversing rows first seems to be slighty faster
    
    array3D(4,3,2);

    return 0;
}