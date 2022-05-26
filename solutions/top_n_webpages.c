#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "top_n_webpages.h"

int is_sorted(double *a, int n) {
    /*  
    Traverses through array and checks if all elements are in ascending order.
    If yes, returns true (1), else returns false (0)
    */
    for (int i=0; i<n-1; i++) {
        if (a[i] > a[i+1]) {
            return 0;
        }
    }
    return 1;
}

void compare_exchange_perm(double *(*a), int x_left, int x_right, int *(*perm)) {
    /*
    Swaps two values of array a if leftmost element is larger than rightmost element.
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
    }
}

void sort_nodes_perm(double *(*a), int N, int *(*perm)) {
    /*
    Sorts provided array a and associated indices
    a: sequential webpage scores
    N: number of edges
    perm: permutation array holding indices of scores
    */
    int i, offset;
    if (N % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;

    *perm = (int*)malloc(N*sizeof(int)); // maximum number of sorts -> may be less depending on when it's done sorting
    for (int i=0; i<N; i++) {
        (*perm)[i] = i; // initialize permutations
    }

    for (i=1; i<=N; i++) {
        if (is_sorted(*a, N) == 1) {
            break; // stop comparing if already sorted
        }
        if (i % 2 == 0) { // even
            for (int j=0; j<N/2-offset; j++) {
                compare_exchange_perm(a, 2*j+1, 2*j+2, perm);
            }
        }
        else {
            for (int j=0; j<N/2; j++) {
                compare_exchange_perm(a, 2*j, 2*j+1, perm);
            }
        }
    }
    printf("\nNr sorting iterations (iters/max): %d/%d\n", i, N);
}

void top_n_webpages(int N, double *scores, int n) {
    /* 
    Uses sorting algorithm to sort scores in ascending order
    and prints indices and scores for top n webpages.
    */
    if (n > N) {
        printf("Failure: Top n (%d) can't exceed the number of webpages (%d).\n", n, N);
        exit(0);
    }
    int *perm;

    sort_nodes_perm(&scores, N, &perm);

    double *top_n_scores = (double*)malloc(n*sizeof(double));
    int *top_n_idx = (int*)malloc(n*sizeof(int));
    // RETURN INDICES OF WEBPAGE ALSO!
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