#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int count_occurence(const char *text_string, const char *pattern) {
    int pattern_len = strlen(pattern);
    int text_len = strlen(text_string);
    int num_matches = 0;
    int i;

    char text_sub[pattern_len];

    // pattern and text_string (and their lengths) are shared among all threads
    // text_sub needs to be parallelized to prevent race condition
    #pragma omp parallel for private(i, text_sub) reduction(+:num_matches)
    for (int k=0; k<text_len-pattern_len+1; k++) {
        for (i=k; i<k+pattern_len; i++) {
            text_sub[i-k] = text_string[i];
        }
        num_matches += !strncmp(text_sub, pattern, pattern_len);
    }

    return num_matches;
}   


int main() {
    const char *my_text = "RABCDEFGHSABCGABCABCODM";
    const char *pattern = "ABC";

    int num = count_occurence(my_text, pattern);
    printf("Number of occurences: %d\n", num);
}
