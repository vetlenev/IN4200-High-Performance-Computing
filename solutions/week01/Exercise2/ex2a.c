#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ex2a_common.h"
#include "minmax.h"
//#include "ex2a_functions.c"

/*
This program has been expanded somewhat to resemble a proper, "big" program.

This seems as good a place as any to introduce structs. A struct is simply a collection
of variables bunched together, so it's not a terribly complicated idea.

It gets a bit messier when we get to structs and struct pointers. Recall that, in Java,
as has hopefully been repeated ad nauseum, whenever you make a declaration like, say,

Scanner thingie;

then you haven't made a Scanner object, but rather a pointer; only when you type

thingie = new Scanner(dot dot dot);

do you make an object. In C, you never make a pointer unless you explicitly
declare it. So upon writing

temperature_data data;

you do make a temperature_data struct. One can then access the struct's variables in familiar fashion:

data.n = 10;

This is, of course, fine for small programs.
If we were to use pointers instead, it would look like this:

temperature_data *data = (temperature_data *)malloc(sizeof(temperature_data));

Data is accessed via ->, e.g.

data->n = 10;

I personally prefer this approach, especially for larger programs. Just remember to use free at the end.
*/

/*double average(double *arr, int n) {

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


void readfile(char *filename, temperature_data *data) { // temperature_data required to specify that pointer *data is a struct-instance of temperature_data

    int err;
    FILE *datafile;

    printf("Filename var: %s\n", filename);
    printf("Filename pointer: %d\n", *filename);
    datafile = fopen(filename, "r");

    if (datafile == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }

    err = fscanf(datafile, "%d", &data->n);
    printf("err: %d \n", err);
    printf("Data struct: %d \n", data->n);

    data->times = (char *)malloc(5 * data->n * sizeof(char));
    data->temps = (double *)malloc(data->n * sizeof(double));

    for (int i = 0; i < data->n; i ++) {
        err = fscanf(datafile, "%5s %lf", &data->times[5 * i], &data->temps[i]);
    }

    fclose(datafile);
}*/

int main(int narg, char **argv) {
    // narg: nr of command-line arguments
    // argv: array (of pointers) of command-line arguments, where each element (argv[i]) is of type *char (not char)
    // argv[0] -> filename
    temperature_data *data;
    int minidx, maxidx;
    double avg, dev;

    if (narg < 2) {

        printf("Filename required.\n");
        exit(0);
    }
    
    data = (temperature_data *)malloc(sizeof(temperature_data));

    readfile(argv[1], data); // argv[1] is a char-pointer
    printf("argv: %d", &argv[1]);

    minidx = findminidx(data->temps, data->n);
    maxidx = findmaxidx(data->temps, data->n);
    avg = average(data->temps, data->n);
    dev = stdev(data->temps, data->n, avg);

    for (int i = 0; i < data->n; i ++) {
        printf("%.5s %lf\n", &data->times[5 * i], data->temps[i]);
    }

    printf("Lowest temperature  %f at time %.5s\n", data->temps[minidx], &data->times[5 * minidx]);
    printf("Highest temperature  %f at time %.5s\n", data->temps[maxidx], &data->times[5 * maxidx]);
    printf("Mean temperature %f, with standard deviation %f.\n", avg, dev);

    free(data->times);
    free(data->temps);
    free(data);

}
