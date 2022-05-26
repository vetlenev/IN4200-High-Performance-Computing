#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "top_n_webpages.h"


int compare_exchange_perm_omp(double *(*a), int x_left, int x_right, int *(*perm)) {
    /*
    Swaps two values of array a if leftmost element is larger than rightmost element.
    Returns 1 if values are swapped, 0 else. (Necessary or OpenMP parallelization).
    a: array to check for swap
    x_left: left index
    x_right: right index (x_right > x_left)
    perm: permutation array to keep track of swapped indices
    */
    if ((*a)[x_left] > (*a)[x_right]) {
        double tmp = (*a)[x_left];
        (*a)[x_left] = (*a)[x_right];
        (*a)[x_right] = tmp;

        int tmp_perm = (*perm)[x_left];
        (*perm)[x_left] = (*perm)[x_right];
        (*perm)[x_right] = tmp_perm;
        
        return 1;
    }
    return 0;
}

void sort_nodes_perm_omp(double *(*a), int N, int *(*perm)) {
    /*
    Sorts provided array a and associated indices using OpenMP
    a: sequential webpage scores
    N: number of edges
    perm: permutation array holding indices of scores
    */
    int offset;
    int even_changed = 0, odd_changed = 0;
    if (N % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;

    *perm = (int*)malloc(N*sizeof(int));
    for (int i=0; i<N; i++) {
        (*perm)[i] = i; // initialize permutations
    }

    int i;
    #pragma omp parallel private(i) // make iterator i private so sorting iterations between threads are independent
    {   
        for (i=1; i<=N; i++) {
            if (i % 2 == 0) { // even
                // Comparison of two values for each iteration are independent => can parallelize
                #pragma omp for reduction(+:even_changed)
                for (int j=0; j<N/2-offset; j++) {
                    even_changed += compare_exchange_perm_omp(a, 2*j+1, 2*j+2, perm);
                }
            }
            else {
                #pragma omp for reduction(+:odd_changed)
                for (int j=0; j<N/2; j++) {
                    odd_changed += compare_exchange_perm_omp(a, 2*j, 2*j+1, perm);
                }
            }

            if (!even_changed && !odd_changed) {
                break; // Stop sorting if no values have been swapped
            }

            #pragma omp barrier // synchronize to prevent some threads to update even_changed/odd_changed before all threads have checked the previous even/odd count
            #pragma omp single
            { // reset for next iteration - only need to be done by one thread
                even_changed = 0; 
                odd_changed = 0;
            }
        }
        #pragma omp single
        {
            printf("\nNr sorting iterations (iters/max): %d/%d\n", i, N);
        }
    }
}

void top_n_webpages_omp(int N, double *scores, int n) {
    /* 
    Uses sorting algorithm to sort scores in ascending order
    and prints indices and scores for top n webpages.
    Parallelized version using OpenMP.
    */
    if (n > N) {
        printf("Failure: Top n (%d) can't exceed the number of webpages (%d).\n", n, N);
        exit(0);
    }

    int *perm;
    
    sort_nodes_perm_omp(&scores, N, &perm);
    
    double *top_n_scores = (double*)malloc(n*sizeof(double));
    int *top_n_idx = (int*)malloc(n*sizeof(int));
    
    #pragma omp parallel for
    for (int i=0; i<n; i++) {
        top_n_scores[i] = scores[N-i-1];
        top_n_idx[i] = perm[N-i-1];
    }

    printf("\nTop %d webpages:\n", n);
    printf("n: Score | Index\n");
    for (int i=0; i<n; i++) {
        printf("%d: %.4lf | ", i+1, top_n_scores[i]);
        printf("%d\n", top_n_idx[i]);
    }
}