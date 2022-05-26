#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
typedef int bool;

float* compare_exchange(float *a, int x_left, int x_right) {
    if (a[x_left] > a[x_right]) {
        float tmp = a[x_left];
        a[x_left] = a[x_right];
        a[x_right] = tmp;
    }
    return a;
}

int compare_exchange_omp(float *(*a), int x_left, int x_right) {
    if (*a[x_left] > *a[x_right]) {
        float tmp = *a[x_left];
        *a[x_left] = *a[x_right];
        *a[x_right] = tmp;
        return 1;
    }
    return 0;
}

bool is_sorted(float *a, int n) {
    for (int i=0; i<n-1; i++) {
        if (a[i] > a[i+1]) {
            return false;
        }
    }
    return true;
}

void serial_odd_even(int n, float *(*a)) { // a is an ARRAY (*) sent in as a POINTER (*)
    for (int i=1; i<=n; i++) {
        if (i % 2 == 0) { // even
            for (int j=0; j<n/2-1; j++) {
                *a = compare_exchange(*a, 2*j+1, 2*j+2);
            }
        }
        else {
            for (int j=0; j<n/2; j++) {
                *a = compare_exchange(*a, 2*j, 2*j+1);
            }
        }
    }
}

int test_sorting_algorithm() {
    float *a = (float*)malloc(8*sizeof(float));
    for (int i=0; i<8; i++) {
        a[i] = 8-i;
    }
    for (int i=0; i<8; i++) {
        printf("%lf ", a[i]);
    }
    printf("\n");

    serial_odd_even(8, &a);

    for (int i=0; i<8; i++) {
        printf("%lf ", a[i]);
    }
    printf("\n");
    float a_sorted[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    for (int i=0; i<8; i++) {
        if (a[i] != a_sorted[i]) {
            printf("Error in sorted array, element %d: %lf != %lf\n", i, a[i], a_sorted[i]);
            return 1; // exit code
        }
    }
    return 0; // all good
}

void serial_odd_even_improved(int n, float *(*a)) {
    int i, offset;
    if (n % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;

    for (i=1; i<=n; i++) {
        if (is_sorted(*a, n) == true) {
            break; // stop comparing if already sorted
        }
        if (i % 2 == 0) { // even
            for (int j=0; j<n/2-offset; j++) {
                *a = compare_exchange(*a, 2*j+1, 2*j+2);
            }
        }
        else {
            for (int j=0; j<n/2; j++) {
                *a = compare_exchange(*a, 2*j, 2*j+1);
            }
        }
    }
    printf("Nr sorting iterations: %d/%d\n", i, n);
}

int test_improved_sorting_algorithm() {
    int n = 9;
    float *a = (float*)malloc(n*sizeof(float));
    for (int i=0; i<n; i++) {
        a[i] = n-i;
    }
    for (int i=0; i<n; i++) {
        printf("%lf ", a[i]);
    }
    printf("\n");

    serial_odd_even_improved(n, &a);

    for (int i=0; i<n; i++) {
        printf("%lf ", a[i]);
    }
    printf("\n");
    float a_sorted[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (int i=0; i<n; i++) {
        if (a[i] != a_sorted[i]) {
            printf("Error in sorted array, element %d: %lf != %lf\n", i, a[i], a_sorted[i]);
            return 1; // exit code
        }
    }
    return 0; // all good
}

void serial_odd_even_omp(int n, float *a) {
    int offset; 
    int even_changed = 0, odd_changed = 0;
    if (n % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;
    printf("Off: %d", offset);

    #pragma omp parallel 
    { // start parallel region
        for (int i=1; i<=n; i++) {
            if (i % 2 == 0) { // even
                #pragma omp for reduction(+:even_changed) // if each thread gets a pair number of iterations each, they will be assigned pairwise elements to compare that are independent of the comparison of another thread
                for (int j=0; j<n/2-offset; j++) {
                    even_changed += compare_exchange_omp(&a, 2*j+1, 2*j+2);
                }
            }
            else {
                #pragma omp for reduction(+:odd_changed)
                for (int j=0; j<n/2; j++) {
                    odd_changed += compare_exchange_omp(&a, 2*j, 2*j+1);
                }
            }
            
            if (!even_changed && !odd_changed) {
                break; // WHY CANT THIS BE INSIDE SINGLE CLAUSE ??
            }

            #pragma omp barrier // SYNCHRONIZATION POINT
            #pragma omp single // no need for all threads to set even_changed and odd_changed to zero, but must make sure they are synchronized at end to prevent other threads from continuing next loop 
            {
                even_changed = 0;
                odd_changed = 0;
            }
            // SYNCHRONIZATION POINT

            /*// is_sorted compares magnitudes of elements sequentially => all elements are dependent on each other -> no room for parallelization
            #pragma omp single 
            {
                #pragma omp task depend(out: done)
                done = is_sorted(*a, n);
            }
            if (done) {
                break;
            }*/
        }
    } // end parallel region
    //printf("Nr sorting iterations: %d/%d\n", i, n);
}
