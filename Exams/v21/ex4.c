#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int *func_serial(int N, int *arr)
{
    int pass, smallIndex, j, temp;
    for (pass = 0; pass < N-1; pass++)
    {
        smallIndex = pass;
        for (j = pass+1; j < N; j++) {
            if (arr[j] < arr[smallIndex])
            smallIndex = j;
        }
        temp = arr[pass];
        arr[pass] = arr[smallIndex];
        arr[smallIndex] = temp;
    }
    return arr;
}

int *func_omp_fail(int N, int *arr)
{
    int pass, smallIndex, smallValue, j, temp;
    // Outer loop can't be parallelized => race condition for inner loop exchanges (dependencies of elements)
    for (pass = 0; pass < N-1; pass++)
    {
        smallIndex = pass;
        smallValue = arr[smallIndex];
        #pragma omp parallel for reduction(min:smallValue)
        for (j = pass+1; j < N; j++) {
            if (arr[j] < smallValue) {
                //smallIndex = j;
                smallValue = arr[j];
            }
        }
        
        temp = arr[pass];
        arr[pass] = smallValue;
        arr[smallIndex] = temp;
    }
    return arr;
}

int *func_omp(int N, int *arr)
{
    int pass, smallIndex, smallValue, j, temp;
    int n_threads = 8;
    int *smallValueStorage = malloc(n_threads*sizeof smallValueStorage);

    #pragma omp parallel private(smallIndex, smallValue, pass) num_threads(n_threads)
    {
    // Outer loop can't be parallelized => race condition for inner loop exchanges (dependencies of elements)
    for (pass = 0; pass < N-1; pass++)
    {
        int my_id = omp_get_thread_num();
        smallIndex = pass;

        #pragma omp for
        for (j = pass+1; j < N; j++) {
            if (arr[j] < arr[smallIndex]) {
                smallIndex = j;
                //smallValue = arr[j];
            }
        }
        // smallValue private for each thread - must compare to find global minimum
        smallValueStorage[my_id] = smallIndex;
        #pragma omp barrier // make sure every thread has assigned to storage to avoid getting non-updated value
        smallIndex = smallValueStorage[0]; // let first thread have the current smallest value
        // Let ONE thread find global smallest value to avoid race condition
        #pragma omp single
        {
        for (int i=1; i<n_threads; i++) {
            if (arr[smallIndex] > arr[smallValueStorage[i]])
                //arr[smallIndex] = arr[smallValueStorage[i]]; // update if another thread has a smaller value
                smallIndex = smallValueStorage[i];
        }
        
        temp = arr[pass];
        arr[pass] = arr[smallIndex];
        arr[smallIndex] = temp;
        }
    }
    }
    return arr;
}

int main() {
    int N = 10;
    int *arr = malloc(N*sizeof arr);
    int *arr2 = malloc(N*sizeof arr2);
    arr[0] = 10; arr[1] = 9; arr[2] = 23; arr[3] = 12; arr[4] = 7;
    arr[5] = 4; arr[6] = 1; arr[7] = 8; arr[8] = 2; arr[9] = 5;
    arr2[0] = 10; arr2[1] = 9; arr2[2] = 23; arr2[3] = 12; arr2[4] = 7;
    arr2[5] = 4; arr2[6] = 1; arr2[7] = 8; arr2[8] = 2; arr2[9] = 5;

    int *arr_s;
    int *arr_p;

    arr_s = func_serial(N, arr);
    arr_p = func_omp(N, arr2);

    for (int i=0; i<N; i++) {
        printf("arr_s[%d] = %d\n", i, arr_s[i]);
        printf("arr_p[%d] = %d\n", i, arr_p[i]);
        if (arr_s[i] != arr_p[i]) {
            printf("Fail: %d: %d != %d\n", i, arr_s[i], arr_p[i]);
            return 1;
        }
    }

    return 0;
}
