#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int i, N=100000;
double u[N], v[N];

#pragma omp parallel for
for (i=0; i<N; i++) {
    u[i] = 0.001*(i-N/2);
    v[i] = 0.0;
}

#pragma omp parallel default(shared)
{
    int time_step;
    double *tmp;

    v[N-1] = u[N-1];

    for (time_step=0; time_step<1000; time_step++)
    {
        #pragma omp for
        for (i=1; i<N-1; i++)
            v[i] = u[i-1]-2*u[i]+u[i+1];

        #pragma omp single
        {
        tmp = v;
        v=u;
        u=tmp;
        }
    }
}