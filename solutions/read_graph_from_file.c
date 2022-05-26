#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_graph_from_file.h"

void read_graph_from_file(const char *filename, int *N, int *(*row_ptr), int *(*col_idx), double *(*val)) {
    /*Reads a webgraph .txt file and transforms the links to a CRS-format.
    Args:
        filename: name of webgraph file
        N: number of webpages/nodes
        row_ptr: index pointer to rows
        col_idx: column index of corresponding values in val
        val: contiguous 1D storage of links in hyperlink matrix
    */
    FILE *fr;
    // Read twice -> once to fill row_ptr, once to fill col_idx and val
    fr = fopen(filename, "r");

    if (fr == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }

    int num_links; // int *num_links will not work, since then we pass an integer pointer as argument to fscanf rather than an address to an actual integer 

    // Read headers
    fscanf(fr, "%*[^\n]\n"); // first line -> ignore
    fscanf(fr, "%*[^\n]\n"); // second line -> ignore
    fscanf(fr, "%*c %*s %d %*s %d \n", N, &num_links); // ignore all strings and special characters, and stop at new line
    fscanf(fr, "%*[^\n]\n"); // fourth line -> ignore
    
    // Read edges (includes self-links)
    int* from_idx_tmp = (int*)calloc(num_links, sizeof(int));
    int* to_idx_tmp = (int*)calloc(num_links, sizeof(int));

    int from, to;
    int num_edges = 0;

    // Find number of valid edges (i.e. no self-links), and append to temporary From/To-node arrays
    for (int k=0; k<(num_links); k++) {
        fscanf(fr, "%d %d", &from, &to); // "%d %d \n"
        // TODO: replace all edges equal N with 0
        if (from != to) { 
            if (from == *N) {
                from = 0;
            }
            else if (to == *N) {
                to = 0;
            }
            from_idx_tmp[k] = from;
            to_idx_tmp[k] = to;
            num_edges++;
        }
    }

    int* from_idx = (int*)malloc(num_edges*sizeof(int));
    int* to_idx = (int*)malloc(num_edges*sizeof(int));
    
    int edge_idx = 0;
    // Recompute edges and append to true From/To-node, omitting self-links
    for (int i=0; i<num_links; i++) {
        if (from_idx_tmp[i] != to_idx_tmp[i]) { // Omit self-links
            from_idx[edge_idx] = from_idx_tmp[i];
            to_idx[edge_idx] = to_idx_tmp[i];
            edge_idx++;
        }
    }

    *val = calloc(num_edges, sizeof(*val));
    *row_ptr = calloc((*N+1), sizeof(*row_ptr)); // +1 to include endpoint
    *col_idx = calloc(num_edges, sizeof(*col_idx));
    int* counter = calloc(*N, sizeof(int)); // count nr nonzero values in each row -> determined by amount of each unique element in to_idx

    double** A = (double**)calloc((*N), sizeof(double*));
    int* L = calloc((*N), sizeof(int));

    A[0] = (double*)calloc((*N)*(*N), sizeof(double));

    for (int i=1; i<(*N); i++) {
        A[i] = &(A[0][i*(*N)]);
    }

    for (int i=0; i<num_edges; i++) {
        L[from_idx[i]] += 1; // number of outbound links from webpage from_idx[i]
    }

    int k = 0;
    for (int j=0; j<num_edges; j++) { // loop through from_idx
        for (int i=0; i<(*N); i++) { // loop through to_idx
            if (to_idx[j] == i) {
                A[i][from_idx[j]] = (double)1/L[from_idx[j]]; // swap i and j in A because to_idx corresponds to inbound links (rows) whil from_idx corresponds to outbound links (cols)
                k++;
            }
        }
    }

    k = 0;
    for (int i=0; i<(*N); i++) {
        for (int j=0; j<(*N); j++) {
            if (A[i][j] != 0.0) {
                (*val)[k] = A[i][j]; // store non-zero elements in hyperlink matrix in contiguous array
                (*col_idx)[k] = j;
                k++;
            }
        }
    }

    if (*N < 20) { // print matrix of not too large
        printf("------ HYPERLINK MATRIX ------\n");
        print_hyperlink(A, *N);
    }

    // Reload file to allocate row_ptr
    rewind(fr);

    for (int i=0; i<4; i++) { // No need for headers
        fscanf(fr, "%*[^\n]\n");
    }

    for (int k=0; k<num_links; k++) {
        fscanf(fr, "%d %d", &from, &to); // need fromNode only to check if self-link
        if (from != to) { // omit self-links
            counter[to] += 1; // count number of inbound links of given webpage
        }
    }

    for (int i=0; i<(*N); i++) {
        (*row_ptr)[i+1] += (*row_ptr)[i] + counter[i];
    } 
}