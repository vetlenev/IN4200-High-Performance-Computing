#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_occurence(const char *text_string, const char *pattern) {
    int pattern_len = strlen(pattern);
    int text_len = strlen(text_string);
    int num_matches = 0;

    for (int k=0; k<text_len-pattern_len+1; k++) { // +1 necessary to include last string
        char text_sub[pattern_len];
        for (int i=k; i<k+pattern_len; i++) {
            text_sub[i-k] = text_string[i];
        }
        
        int match = !strncmp(text_sub, pattern, pattern_len);
        num_matches += match;
    }

    return num_matches;
}   


int main() {
    const char my_text[24] = "RABCDEFGHSABCGABCABCABC";
    const char pattern[4] = "ABC";
    //printf("frfe: %s\n", &my_text[3]);

    int num = count_occurence(my_text, pattern);
    printf("Number of occurences: %d\n", num);
}
