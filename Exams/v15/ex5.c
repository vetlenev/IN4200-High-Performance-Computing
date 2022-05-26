#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIGN(x) ((x < 0)? -1 : (x > 0))
#define POSITIVE(x) ((x < 0)? 0 : x)

int count_friends_of_ten(int M, int N, int **v) {
    int i, j, r, c, r_tmp, c_tmp;
    int single_TVT;
    int num_TVT = 0; // number of ten-friends-triples
    // Go through each number in table
    for (i=0; i<M; i++) {
        for (j=0; j<N; j++) {
            // For each element, sum all possible three-neighbors and append to num_TVT
            /*
            if corner:
                no valid TVT
            else if edge:
                if (i==0 | i==m-1)
                    horizontal TVT
                elif (j==0 | j==n-1)
                    vertical TVT
            else:
                4 TVTs: 1 vertical, 1 horizontal, 2 diagonals
            */
        }
    }
    // Each TVT has been computed twice (from element at either end of triplet)
    // => get correct count by dividing total number by 2
    num_TVT = num_TVT * 0.5; // multiply faster than division
    return num_TVT;
}

int main() {
    int m=4;
    int n=5;

    int **v = malloc(m*sizeof *v);
    v[0] = malloc(m*n*sizeof v);

    for (int i=1; i<m; i++) {
        v[i] = &v[0][i*n];
    }
    
    v[0][0] = 1; v[0][1] = 5; v[0][2] = 6; v[0][3] = 3; v[0][4] = 1;
    v[1][0] = 2; v[1][1] = 4; v[1][2] = 4; v[1][3] = 1; v[1][4] = 7;
    v[2][0] = 7; v[2][1] = 2; v[2][2] = 3; v[2][3] = 1; v[2][4] = 2;
    v[3][0] = 3; v[3][1] = 2; v[3][2] = 2; v[3][3] = 5; v[3][4] = 3;

    //printf("v: %d\n", v[0][-1]);
    int num_TVT = count_friends_of_ten(m, n, v);

    return 0;
}