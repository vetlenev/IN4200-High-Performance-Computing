#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

void smooth(double **v_new, double **v, int c, int nx, int ny) {
    // The parameters v_new and v are not formal, they are pointers hence can be changed in function
    int i, j;
    for (i=1; i<nx-1; i++) {
        for (j=1; j<ny-1; j++) {
            v_new[i][j] = v[i][j] + c*(v[i-1][j] + v[i][j-1] - 4*v[i][j] + v[i][j+1] + v[i+1][j]);
        }
    }
    v_new[0][0] = v[0][0];
    v_new[nx-1][ny-1] = v[nx-1][ny-1];
}


void tempStats(char *filename) { // make filename pointer to be able to retrieve both name of file and address
    FILE *fr; // file pointer to read file in associated address
    fr = fopen(filename, "r");

    //char store_time[1000];
    struct stat sb;
    stat(filename, &sb);
    //char *store_time = malloc(sb.st_size);
    char store_time[100][5]; // pointers to 100 char elements each of max size 5

    float store_temp[100];
    int i=0;
    int j=0;

    //while (fscanf(fr, "%f", &store_temp[i++]) != EOF);
    while (!feof(fr)) {
        fscanf(fr, "%s", store_time[i]); // for char-arrays we don't need to read from the address - can directly extract the string
        fscanf(fr, "%f", &store_temp[j]);
        i++;
        j++;
    }
    fclose(fr);

    float sum = 0, avg, min=INFINITY, max=-INFINITY;
    int k, k_min, k_max;
    char min_date; 
    char max_date;

    for (k=0; k<j-1; k++) { // The last i++ does not have associated element => k<j-1 instead of k<j
        sum += store_temp[k];
        if (store_temp[k] < min) {
            min = store_temp[k];
            k_min = k;
        }
        if (store_temp[k] > max) { // can't use elif -> this would ignore the first element since this would always be less than min=-infty
            max = store_temp[k];
            k_max = k;
        }
    }

    avg = sum/k;
    printf("Average temp: %f\n", avg);
    printf("Min temp: %f, occured at time: %s\n", min, store_time[k_min]);
    printf("Max temp: %f, occured at time: %s\n", max, store_time[k_max]);
}


int main() {
    int c=2, nx=10, ny=10;
    double **arr_new = (double**)malloc(nx*sizeof(double*));
    double **arr = (double**)malloc(nx*sizeof(double*));

    for (size_t i=0; i<nx; i++) {
        // Addresses must be allocated to BOTH arrays before they are manipulated
        arr_new[i] = (double*)malloc(ny*sizeof(double));
        arr[i] = (double*)malloc(ny*sizeof(double));

        for (size_t j=0; j<ny; j++) {
            arr[i][j] = i*ny*nx + j*ny*i; // Only need assigning value to arr, arr_new is to be computed later
        }
    }

    smooth(arr_new, arr, c, nx, ny);

    printf("Old array (1,2): %lf\n", arr[1][5]);
    printf("New array (1,2): %lf\n", arr_new[1][5]);

    char *tempFile = "temps.txt";
    tempStats(tempFile);

    return 0;
}