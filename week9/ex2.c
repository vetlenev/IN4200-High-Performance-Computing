#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

void error_code() {
    int i, N=100000;
    double u[N], v[N];

    for (i=0; i<N; i++) {
        u[i] = 0.001*(i-N/2);
        v[i] = 0.0;
    }

    #pragma omp parallel default(shared)
    {
        int time_step;
        double *tmp;

        for (time_step=0; time_step<1000; time_step++) {
            #pragma omp for nowait
            for (i=1; i<N-1; i++) {
                v[i] = u[i-1] - 2*u[i] + u[i+1];
            }
            // All values of v are updated, except boundaries v[0] and v[N-1].
            // These must be updated (by single thread) to ensure correct update for all values of u.
            // Otherwise, boundaries of u will not be smoothed, only set to 0, creating a discontinuous gap to internal elements.
            tmp = v;
            v = u;
            u = tmp;
        }

    }
}


void optimized_code() {
    int i, N=100000;
    //double u[N], v[N]; // static allocation of array -> allocated on stack => inefficient access by threads
    double *u = (double*)malloc(N*sizeof(double)); 
    double *v = (double*)malloc(N*sizeof(double)); // Allocation done on heap => all threads have possibility to update

    #pragma omp parallel for
    for (i=0; i<N; i++) {
        u[i] = 0.001*(i-N/2);
        v[i] = 0.0;
    }

    #pragma omp parallel default(shared)
    {
        int time_step;
        double *tmp;

        #pragma omp for private(i) // time_step automatically made private, i is not since it's inner loop
        for (time_step=0; time_step<1000; time_step++) {
            //#pragma omp for nowait // nowait will prevent synchronization at end of for-loop, risking one of threads to update v and u before other threads are done with computation
            #pragma omp for
            for (i=1; i<N-1; i++) {
                v[i] = u[i-1] - 2*u[i] + u[i+1];
            }
            // Synchronization at this point anyway
            #pragma omp single // if using master, it may happen that some threads proceed to computation in next iteration before single thread is finished updating v an u. Also, if multiple threads do the swapping, we may end up with wrong update
            {
                // Missing endpoint updates -> this can also be done by single
                v[0] = u[0];
                v[N-1] = u[N-1];
                tmp = v;
                v = u;
                u = tmp; // boundary conditions of u secured in the swap of values to v --> otherwise boundaries of u would just be set to 0 (original values of v, not the actual dynamic boundaries of u)
            }
        }

    }
}