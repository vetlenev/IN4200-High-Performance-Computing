#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define MIN(a, b) ((a<b)?a:b) 

double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void floyd(double *(**a), int n) {
    for (int k=0; k<n; k++)
        for (int i=0; i<n; i++)
            for (int j=0; j<n; j++)
                if ( (*a)[i][j] > ((*a)[i][k]+(*a)[k][j]) )
                    (*a)[i][j] = (*a)[i][k]+(*a)[k][j];

}

void floyd_omp(double *(**a), int n) {
    // All three nested loops are independent of each other => all can be parallelized
    // But innermost loop has a minimum check between array elements => dependency!
    //#pragma omp parallel for private(i, j)
    double a_val;
    int i, j, k;
    // Loop iterations are independent, so order of loop doesn't matter
    for (k=0; k<n; k++) {
        // Outer loop can't be parallelized because we risk different threads checking the same a[i][j] simultaneously - causing a potential race condition when finding the minima
        #pragma omp parallel for private(j) // Indices i and j are made private to 
        for (i=0; i<n; i++) {
            for (j=0; j<n; j++) {
                //a_val = (*a)[i][j];
                //#pragma omp for //reduction(min:a_val)
                // Each iteration checks different (independent) values => parallelizable. Just make sure to use reduction on a_val to avoid race condition
                if ( (*a)[i][j] > ((*a)[i][k]+(*a)[k][j]) )
                    (*a)[i][j] = (*a)[i][k]+(*a)[k][j];
            }
            //(*a)[i][j] = a_val;
        }
    }
}

double **floyd_omp_new(double **A, int n) {
    double **D = malloc(n*sizeof *D);
    D[0] = malloc(n*n*sizeof D);
    for (int i=1; i<n; i++) {
        D[i] = &D[0][i*n];
    }
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            D[i][j] = MIN(A[i][j], A[j][i]); // Shortest direct graph
        }
    }

    int i, j, k;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            for (k=0; k<n; k++) {
                if (A[j][k] > A[j][i]+A[i][k]) // Direct distance longer than indirect distance
                    D[j][k] = A[j][i] + A[i][k];
            }
        }
    }
    /*
    Even though we assign to a new array D (getting rid of race condition for A),
    we still have the problem 
    */
   return D;
}

double **floyd_mpi(double *(**a), int n, int argc, char *argv[]) {
    int i, j, k;
    int my_rank, num_procs;

    double **D = malloc(n*sizeof *D);
    D[0] = malloc(n*n*sizeof D);
    for (int i=1; i<n; i++) {
        D[i] = &D[0][i*n];
    }
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            D[i][j] = MIN((*a)[i][j], (*a)[j][i]); // Shortest direct graph
        }
    }
    // Loop iterations are independent, so order of loop doesn't matter
    for (k=0; k<n; k++) { // k shared variable among processes
        MPI_Init(&argv, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        for (i=0; i<n; i++) {
            for (j=0; j<n; j++) {
                if (D[i][j] > (D[i][k] + D[k][j]))
                    D[i][j] = D[i][k] + D[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int n=10;
    double **a = malloc(n*sizeof(double*));
    double **a_omp = malloc(n*sizeof(double*));
    a[0] = malloc(n*n*sizeof(double));
    a_omp[0] = malloc(n*n*sizeof(double));

    for (int i=1; i<n; i++) {
        a[i] = &a[0][i*n];
        a_omp[i] = &a_omp[0][i*n];
    }
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            double rand_val = randfrom(0., 1.);
            a[i][j] = rand_val;
            a_omp[i][j] = rand_val;
        }
    }

    double **D = floyd_mpi(a, n, argc, argv); // a not overwritten here
    floyd(&a, n);
    floyd_omp(&a_omp, n);

    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            //printf("a[%d][%d] = %lf\n", i, j, a[i][j]);
            //printf("a_omp[%d][%d] = %lf\n", i, j, a_omp[i][j]);
            if (a[i][j] != D[i][j]) {
                printf("FAIL\n");
                return 1;
            }
        }
    }

    return 0;
}
