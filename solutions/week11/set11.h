#ifndef SET11_H
#define SET11_H


// Return array with n values evenly spaced in the intervall [start, stop].
// Both endpoints are included in the returned array.
//
// This function deals in ints. So the spacing might vary if the size of the
// intervall is not divisible by n.
int* linspace(int start, int stop, int n){
    int *samples = malloc(n * sizeof *samples);
    samples[0] = start;
    samples[n-1] = stop;

    double step = (double)(stop - start)/n;
    double sample = start + step;

    for (size_t i = 1; i < n-1; i++) {
        samples[i] = sample;
        sample += step;
    }

    return samples;
}

#endif
