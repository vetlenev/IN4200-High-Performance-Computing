#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>

void compare_exchange(int i, int j, int *(*a)) {
    if ((*a)[i] > (*a)[j]) {
        int tmp = (*a)[i];
        (*a)[i] = (*a)[j];
        (*a)[j] = tmp;
    }
}

int is_sorted(int n, int *a) {
    for (int i=0; i<n-1; i++) {
        if (a[i] > a[i+1])
            return 0;
    }
    return 1;
}

int *serial_bubble_sort(int n, int *a) {
    int sorted = 0;
    for (int i=n-1; i>0; i--) {
        sorted = is_sorted(n, a);
        if (sorted)
            break;
            
        for (int j=0; j<i; j++) {
            compare_exchange(j, j+1, &a);
        }
    }
    return a;
}

int main() {
    int a[8] = {5, 7, 1, 9, 2, 4, 3, 8};

    int *a_sorted = serial_bubble_sort(8, a);
    for (int i=0; i<8; i++) {
        printf("a[%d] = %d\n", i, a_sorted[i]);
    }

    return 0;
}