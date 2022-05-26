#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

void read_file(char *filename, int *num_links, int *(*from_idx), int *(*to_idx)) { // char* => single character. char** => full string
    // num_links: total number of links between nodes. Is a pointer to integer -> can be changed in-place (*)
    // from_idx: (array) source node/webpage for every link -> changed in-place (**)
    // to_idx: (array) destination node/webpage for every link -> changed in-place (**)
    FILE *fr;
    fr = fopen(filename, "r");

    if (fr == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }
    int nr_lines = *num_links + 4; // add four header lines    

    int num_nodes;

    // Read headers
    fscanf(fr, "%*[^\n]\n"); // first line -> ignore
    fscanf(fr, "%*[^\n]\n"); // second line -> ignore
    fscanf(fr, "%*c %*s %d %*s %d \n", &num_nodes, num_links); // ignore all strings and special characters, and stop at new line
    fscanf(fr, "%*[^\n]\n"); // fourth line -> ignore

    // Read linkages
    *from_idx = malloc(*num_links*sizeof(*from_idx));
    *to_idx = malloc(*num_links*sizeof(*to_idx));
    int from, to;

    for (int k=0; k<(*num_links); k++) {
        fscanf(fr, "%d %d", &from, &to);
        (*from_idx)[k] = from;
        (*to_idx)[k] = to;
    }

    printf("From idx: %d", (*to_idx)[5]);
    
    fclose(fr);
}

int main() {
    char* web_file = "simple-webgraph.txt";
    int num_links = 17;
    int* from_idx = (int*)calloc(num_links, sizeof(int));
    int* to_idx = (int*)calloc(num_links, sizeof(int));
    //int from_idx[num_links];
    //int to_idx[num_links];

    read_file(web_file, &num_links, &from_idx, &to_idx);

    return 0;
}