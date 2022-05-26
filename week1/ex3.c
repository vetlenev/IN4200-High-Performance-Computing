#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_perm(int arr[], int perm[], int beg, int end) {

    if (end > beg + 1) {
        int piv = arr[beg], l = beg + 1, r = end;
        int ppiv = arr[perm[beg]];
        while (l < r) {
            if (arr[perm[l]] <= ppiv)
                l++;
            else
                swap(&perm[l], &perm[--r]); // Important to swap addresses, not values
        }
        swap(&perm[--l], &perm[beg]); // &arr[--l] = &arr[l] ???
        sort_perm(arr, perm, beg, l);
        sort_perm(arr, perm, r, end);
    }
}


void sort(int arr[], int beg, int end) {
    //printf("Bool: %d\n", (end > beg + 1));
    if (end > beg + 1) {
        int piv = arr[beg], l = beg + 1, r = end;
        while (l < r) {
            if (arr[l] <= piv)
                l++;
            else
                swap(&arr[l], &arr[--r]); // Important to swap addresses, not values
        }
        swap(&arr[--l], &arr[beg]); // &arr[--l] = &arr[l] ???
        //printf("L1: %d\n", l);
        //printf("R1: %d\n", r);
        sort(arr, beg, l);
        //printf("Beg: %d\n", beg);
        //printf("End: %d\n", end);
        //printf("L: %d\n", l);
        //printf("R: %d\n\n", r);
        sort(arr, r, end);
    }
}

int main() {

    int arr[8] = {4, 5, 3, 6, 2, 8, 1, 7};

    //int *perm = (int*)malloc(n*sizeof(int));
    int perm[8], i;
    for (i=0; i<8; i++) {
        perm[i] = i;
    }

    printf("perm1 %d\n", perm[0]);
    printf("perm2 %d\n", perm[1]);
    printf("perm3 %d\n", perm[2]);
    printf("perm4 %d\n", perm[3]);
    printf("perm5 %d\n", perm[4]);
    printf("perm6 %d\n", perm[5]);
    printf("perm7 %d\n", perm[6]);
    printf("perm8 %d\n\n", perm[7]);

    //sort(arr, 0, 8);
    sort_perm(arr, perm, 0, 8);

    printf("perm1 %d\n", perm[0]);
    printf("perm2 %d\n", perm[1]);
    printf("perm3 %d\n", perm[2]);
    printf("perm4 %d\n", perm[3]);
    printf("perm5 %d\n", perm[4]);
    printf("perm6 %d\n", perm[5]);
    printf("perm7 %d\n", perm[6]);
    printf("perm8 %d\n\n", perm[7]);

    return 0;
}