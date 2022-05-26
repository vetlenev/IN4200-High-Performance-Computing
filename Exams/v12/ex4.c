#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define _USE_MATH_DEFINES

double *serial(int N, double a, double dt, double final_T) {
    double *u=(double*)malloc((N+2)*sizeof(double));
    double *u_prev=(double*)malloc((N+2)*sizeof(double));
    double *tmp_ptr;
    for (int i=0; i<=N+1; i++)
        u_prev[i]=sin(M_PI*i/(N+1));

    double t = 0.0;
    while (t < final_T) {
        for (int i=1; i<=N; i++)
            u[i]=u_prev[i]+a*(u_prev[i-1]-2*u_prev[i]+u_prev[i+1]);

        u[0] = 0.0;
        u[N+1] = 0.0;

        t += dt;
        tmp_ptr = u_prev;
        u_prev = u;
        u = tmp_ptr;
    }
    return u_prev; // the newly updated one
}

double *parallel(int N, double a, double dt, double final_T) {
    double *u = (double*)malloc((N+2)*sizeof(double));
    double *u_prev = (double*)malloc((N+2)*sizeof(double));
    double t;
    double *tmp_ptr;
    int i;
    #pragma omp parallel
    {
    #pragma omp for
    for (i=0; i<=N+1; i++) // trivial to parallelize
        u_prev[i]=sin(M_PI*i/(N+1));

    #pragma omp single
    {
    t = 0.0; // t is shared, so only one thread needs to initialize it
    }
    while (t < final_T) {
        // trivial to parallelize for-loop <- no dependency / race condition
        #pragma omp for
        for (i=1; i<=N; i++)
            u[i]=u_prev[i]+a*(u_prev[i-1]-2*u_prev[i]+u_prev[i+1]);
        
        #pragma omp single
        {
        u[0] = 0.0;
        u[N+1] = 0.0;
        t += dt; // t is not written to anywhere else, so safe to let only one thread update it
        
        tmp_ptr = u_prev;
        u_prev = u;
        u = tmp_ptr;
        }
        // Synchronization at end, so no risk of some threads checking while loop before time has been updated.
    }
    }
    return u_prev;
}


int main() {
    int N = 100;
    double a = 0.2;
    double dt = 0.01;
    double final_T = 1;

    double *u_s;
    u_s = serial(N, a, dt, final_T);
    double *u_p;
    u_p = parallel(N, a, dt, final_T);

    for (int i=0; i<N+2; i++) {
        if (u_s[i] != u_p[i]) {
            printf("Fail on element (%d): %lf != %lf\n", i, u_s[i], u_p[i]);
            return 1;
        }
    }

    return 0;
}