#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

void sweep (int N, double **table1, int n, double **mask, double**table2)
{
    int i,j,ii,jj;
    double temp;¨
    
    #pragma omp parallel for private(j, ii, jj, temp) reduction(+:temp) // outer loop variables automatically made private
    for (i=0; i<=N-n; i++)
        for (j=0; j<=N-n; j++) {
            temp = 0.0; 
            for (ii=0; ii<n; ii++)
                for (jj=0; jj<n; jj++)
                    temp += table1[i+ii][j+jj]*mask[ii][jj];
            table2[i][j] = temp;
        }
}