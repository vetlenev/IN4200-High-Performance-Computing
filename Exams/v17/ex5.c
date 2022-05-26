#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int *dijkstras(int **w, int n) {
    double *d = malloc(n*sizeof d);
    for (int i=0; i<n; i++) {
        for (int j=0; j<i+1; j++) { // symmetric matrix => only need to loop through lower diagonal
            shortest = INF; // Variable to hold the shortest value found so far between source i and node j
            // Loop through all indirect distances between i and j,
            // storing the minimum distance found so far.
            d[i] = min(d[i], new_path)
        }
    }
}

int main(int argc, char *argv[]) {
    n = 6;
    int **w = malloc(n*sizeof *w);
    w[0] = malloc(n*n*sizeof w);
    for (int i=1; i<n; i++)
        w[i] = &w[0][i*n];

    w[0][0] = 0; w[0][1] = 40; w[0][2] = 15; w[0][3] = -1; w[0][4] = -1; w[0][5] = -1;
    w[1][0] = 40; w[1][1] = 0; w[1][2] = 20; w[1][3] = 10; w[1][4] = 25; w[1][5] = 6;
    w[2][0] = 15; w[2][1] = 20; w[2][2] = 0; w[2][3] = 100; w[2][4] = -1; w[2][5] = -1;
    w[3][0] = -1; w[3][1] = 10; w[3][2] = 100; w[3][3] = 0; w[3][4] = -1; w[3][5] = -1;
    w[4][0] = -1; w[4][1] = 25; w[4][2] = -1; w[4][3] = -1; w[4][4] = 0; w[4][5] = 8;
    w[5][0] = -1; w[5][1] = 6; w[5][2] = -1; w[5][3] = -1; w[5][4] = 8; w[5][5] = 0;

    double d = dijkstras(w, n);

    return 0;
}