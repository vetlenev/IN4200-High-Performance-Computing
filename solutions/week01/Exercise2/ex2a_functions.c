#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ex2a_common.h"
#include "minmax.h"

double average(double *arr, int n) {

    double sum = 0;

    for (int i =0; i < n; i ++) sum += arr[i];

    return sum / (double)n; // since n is int, it must be converted to double for this to return a double
}

double stdev(double *arr, int n, double average) {

    double dev = 0;
    double term;

    for (int i = 0; i < n; i ++) {
        term = arr[i] - average;
        dev += term * term;
    }

    dev = sqrt(dev/(double)(n - 1));

    return dev;
}

int findminidx(double *arr, int n) {

    int minidx;
    double min;

    minidx = 0;
    min = arr[0];
    for (int i = 0; i < n; i ++) {
        minidx = MINIDX(minidx, i, arr);
    }

    return minidx;
}

int findmaxidx(double *arr, int n) {

    int maxidx;
    double max;

    maxidx = 0;
    max = arr[0];
    for (int i = 0; i < n; i ++) {
        maxidx = MAXIDX(maxidx, i, arr);
    }

    return maxidx;
}


void readfile(char *filename, temperature_data *data) {

    int err;
    FILE *datafile;

    printf("Filename var: %s\n", filename); // even though filename is pointer argument (*filename), the supplied argument upon invokation is the filename-string, not the address, so filename will simply be the filename while *filename is its address
    printf("Filename pointer: %d\n", *filename);
    //printf("Test: %d\n", test);

    datafile = fopen(filename, "r");

    if (datafile == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }

    err = fscanf(datafile, "%d", &data->n);
    printf("err: %d \n", err);
    printf("Data struct: %d \n", data->n);

    data->times = (char *)malloc(5 * data->n * sizeof(char)); // five is max number of characters a time of format XX:XX can contain
    data->temps = (double *)malloc(data->n * sizeof(double));

    for (int i = 0; i < data->n; i ++) { // run through each line
        err = fscanf(datafile, "%5s", &data->times[i*5]);
        err = fscanf(datafile, "%lf", &data->temps[i]);
        //err = fscanf(datafile, "%5s %lf", &data->times[5 * i], &data->temps[i]); // five address slots set off to each time-element (because each char is reserved for one character), only one for temp (because this is just a single long-float)
    }

    fclose(datafile);
}