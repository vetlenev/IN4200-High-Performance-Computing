#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

double *dense_mat_vec(int m_start, int m_stop, int n, double *x, double *A, double *y)
{
    int i, j;
    for (i=m_start; i<m_stop; i++)
    {
        double tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j]*y[j];
        x[i] = tmp;
    }

    return x;
}

int main() {
    int m=10, n=5;
    double *A = malloc(m*n*sizeof *A);
    double *x = malloc(m*sizeof x);
    double *y = malloc(n*sizeof y);

    for (int i=0; i<n; i++) {
        y[i] = i*(i%3);
    }
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            A[i*n+j] = i%2 + (j%4)*i;
        }
    }

    double *x_full = malloc(m*sizeof x_full);
    double *x_part = malloc(m*sizeof x_part);
    // Do rowwise partition of A, x shared among all threads
    #pragma omp parallel
    {
        int p;
        #pragma omp single
        {
        int p = omp_get_num_threads();
        }
        int my_thread = omp_get_thread_num();
        //int my_m = my_thread*m/p + ((my_thread < m%p)?1:0);
        int my_m_start = my_thread*m/p;
        int my_m_stop = (my_thread+1)*m/p;
        double *my_x = malloc((my_m_stop - my_m_start)*sizeof my_x);
        dense_mat_vec(my_m_stop - my_m_start, n, &x[my_start], A[my_start*n], y);
    }
    return 0;
}
