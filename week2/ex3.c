#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

int num_3D_calc(int nx, int ny, int nz) {
    double*** u = (double***)malloc(nz*sizeof(double**));
    double*** v = (double***)malloc(nz*sizeof(double**));

    // Contiguous allocation of u
    u[0] = (double**)malloc(nz * ny * sizeof(double*)); // Inner array malloced -> must be freed later!
    v[0] = (double**)malloc(nz * ny * sizeof(double*));
    u[0][0] = (double*)malloc(nz * ny * nx * sizeof(double));
    v[0][0] = (double*)malloc(nz * ny * nx * sizeof(double));

    for (int i=1; i<nz; i++) {
        u[i] = &u[0][i*ny]; // ny is length of each inner array, where total number of inner arrays is ny*nz
        v[i] = &v[0][i*ny];
    }

    for (int j=1; j<ny*nz; j++) {
        u[0][j] = &u[0][0][j*nx];
        v[0][j] = &v[0][0][j*nx];
    }
    // u -> 3D array of nx*ny*nz total elements (nx rows, ny cols, nz cells)
    // u[i] -> each cell of u, 2D array of total nx*ny elements (nx rows, ny cols)
    // u[i][j] -> each column of u, 1D array of total nx elements (nx rows)

    // Initialize v and u
    clock_t timeInit0 = clock();
    for (int i=0; i<nx; i++) {
        for (int j=0; j<ny; j++) {
            for (int k=0; k<nz; k++) {
                v[i][j][k] = 2.0 + sin(i*j*k*M_PI / ((nx-1)*(ny-1)*(nz-1)));
                u[i][j][k] = 0.0;
            }
        }
    }
    clock_t timeInit1 = (double)(clock() - timeInit0) / CLOCKS_PER_SEC;
    double initFlos = nx*ny*nz*10; // includes evaluation of sin-argument (if not multiply by 9), does not include assignment
    int initFLOPS = (int)(initFlos/timeInit1);
    
    int *memcpy(double *dest, double *src, size_t n); // Function for copying array elements

    // Compute u using formula
    printf("u[5][5][5] before: %lf\n", v[9][7][5]);
    printf("u[5][5][6] before: %lf\n", v[9][7][6]);

    clock_t timeComp0 = clock();
    for (int i=1; i<nx-1; i++) {
        for (int j=1; j<ny-1; j++) {
            for (int k=1; k<nz-1; k++) {
                u[i][j][k] = v[i][j][k] + (v[i-1][j][k] + v[i][j-1][k] 
                                        + v[i][j][k-1] - 6*v[i][j][k] + 
                                        v[i+1][j][k] + v[i][j+1][k] + v[i][j][k+1])/6;
                memcpy(&v[i][j][k], &u[i][j][k], sizeof(double)); // memcpy can only copy elements through pointers, so we need to provide the addresses - and each element occupies nr space equal to size of its datatype (here: double)
            }
        }
    }
    clock_t timeComp1 = (double)(clock() - timeComp0) / CLOCKS_PER_SEC;
    double compFlos = nx*ny*nz*9;
    double compFLOPS = (compFlos/timeComp1);
    // Assert if some smoothing of v has been done
    printf("u[5][5][5] after: %lf\n", v[9][7][5]);
    printf("u[5][5][6] after: %lf\n", v[9][7][6]);

    int FLOPS = 0.5*(initFLOPS + compFLOPS);
    printf("FLOPS of 3D-program: %lf\n", compFLOPS);

    // Deallocate
    free(v);
    free(v[0]);
    free(v[0][0]);
    free(u);
    free(u[0]);
    free(u[0][0]);

    return 0;
}

int main() {
    int nx, ny, nz, i, j;
    nx = ny = nz = 100;
    num_3D_calc(nx, ny, nz);

    /* Measure FLOPS of 3D solver:
    - Measure time it takes for one iteration to finish (averaged over all iterations), both for initialization and for computation (i.e. one average time for initialization iteration and one for computation iteration)
    - NB: must omit the copying by memcpy, because this is not a floating-point operation but an assignment!
    - NB2: remember to simplify the complex initialization expression and include all FLOPs
    - Divide the average time by nr of floating point operations performed during one iteration - for both initialization and computation
    */

    return 0;
}