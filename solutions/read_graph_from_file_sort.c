#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_graph_from_file.h"

int check_sort(int *a, int n) {
    /*  
    Traverses through array and checks if all elements are in ascending order.
    If yes, returns true (1), else returns false (0)
    */
    for (int i=0; i<n-1; i++) {
        if (a[i] > a[i+1]) {
            return 0;
        }
    }
    return 1;
}

void compare_swap(int *(*a), int*(*b), int x_left, int x_right) {
    /*
    Swaps two values of array a if leftmost element is larger than rightmost element.
    a: array to check for swap
    b: swap elements in this array according to sorting of a
    x_left: left index
    x_right: right index (x_right > x_left)
    */
    if ((*a)[x_left] > (*a)[x_right]) {
        int tmp_a = (*a)[x_left];
        (*a)[x_left] = (*a)[x_right];
        (*a)[x_right] = tmp_a;

        int tmp_b = (*b)[x_left];
        (*b)[x_left] = (*b)[x_right];
        (*b)[x_right] = tmp_b;
    }
}

void sort_array(int *(*a), int *(*b), int N) {
    /*
    Sorts provided array a in ascending order. 
    Elements in array b are swapped correspondingly.
    a: array to sort
    b: array to swap same indices that sorted array a
    N: number of edges
    */
    int i, offset;
    if (N % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;

    for (i=1; i<=N; i++) {
        if (check_sort(*a, N) == 1) {
            break; // stop comparing if already sorted
        }
        if (i % 2 == 0) { // even
            for (int j=0; j<N/2-offset; j++) {
                compare_swap(a, b, 2*j+1, 2*j+2);
            }
        }
        else {
            for (int j=0; j<N/2; j++) {
                compare_swap(a, b, 2*j, 2*j+1);
            }
        }
    }
}

void compare_swap_sub(int *(*a), int x_left, int x_right) {
    /*
    Swaps two values of array a if leftmost element is larger than rightmost element.
    a: array to check for swap
    x_left: left index
    x_right: right index (x_right > x_left)
    */
    if ((*a)[x_left] > (*a)[x_right]) {
        int tmp_a = (*a)[x_left];
        (*a)[x_left] = (*a)[x_right];
        (*a)[x_right] = tmp_a;
    }
}

void sort_sub_array(int *(*a), int idx_start, int idx_stop, int N_sub) {
    /*
    Sorts the part of array a only between the indices idx_start and idx_stop
    a: sequential webpage scores
    idx_start: start index to sort
    idx_stop: last index to sort
    N_sub: number of sequential elements in array to sort
    */
    int i, offset;
    if (N_sub % 2 == 0) { // even array
        offset = 1; // if even: one less pair to be checked
    }
    else offset = 0;

    int *a_sub = malloc(N_sub*sizeof(int));
    for (int i=idx_start; i<idx_stop; i++) {
        a_sub[i-idx_start] = (*a)[i];
    }

    for (i=1; i<=N_sub; i++) {
        if (check_sort(a_sub, N_sub) == 1) {
            break; // stop comparing if already sorted
        }
        if (i % 2 == 0) { // even
            for (int j=0; j<N_sub/2-offset; j++) {
                compare_swap_sub(&a_sub, 2*j+1, 2*j+2);
            }
        }
        else {
            for (int j=0; j<N_sub/2; j++) {
                compare_swap_sub(&a_sub, 2*j, 2*j+1);
            }
        }
    }
    
    // Change original a in-place:
    for (int i=idx_start; i<idx_stop; i++) {
        (*a)[i] = a_sub[i-idx_start];
    }
}

void read_graph_from_file_sort(const char *filename, int *N, int *(*row_ptr), int *(*col_idx), double *(*val)) {
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

    // Find number of edge NOT self-links
    for (int k=0; k<(num_links); k++) {
        fscanf(fr, "%d %d", &from, &to); // "%d %d \n"
        //if (from != to) { // consider self-links
        if (from != to && from != *N && to != *N) { // 
            from_idx_tmp[k] = from;
            to_idx_tmp[k] = to;
            num_edges++;
        }
    }

    int* from_idx = (int*)malloc(num_edges*sizeof(int));
    int* to_idx = (int*)malloc(num_edges*sizeof(int));
    
    int edge_idx = 0;
    // Recompute set of edges, omitting self-links
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

    int* L_out = calloc((*N), sizeof(int));
    int* L_in = calloc((*N), sizeof(int));

    printf("Before sorting From...\n");
    // Sort to_idx and from_idx by ascending indices of to_idx
    sort_array(&to_idx, &from_idx, num_edges);
    printf("After sorting From...\n");

    for (int i=0; i<num_edges; i++) {
        L_out[from_idx[i]] += 1; // number of outbound links from a given webpage
        L_in[to_idx[i]] += 1; // number of inbound links
    }

    // After shaving sorted to_idx, sort the corresponding from_idx.
    // This is necessary to construct val without having to create entire matrix A.
    int k_node = 0, k_edge = 0; // one iterator for nodes, one iterator for edges
    printf("Before sorting To...\n");
    while (k_node < (*N)) {
        if (L_in[k_node] > 1) {
            sort_sub_array(&from_idx, k_edge, L_in[k_node]+k_edge, L_in[k_node]);
        }
        k_edge+=L_in[k_node];
        k_node++;
    }
    printf("After sorting To...\n");

    for (int j=0; j<num_edges; j++) { // loop through from_idx
        int out_link = from_idx[j];
        (*val)[j] = (double)1/L_out[out_link];
        (*col_idx)[j] = from_idx[j];
    }

    // Reload file to allocate row_ptr
    rewind(fr);

    for (int i=0; i<4; i++) { // No need for headers
        fscanf(fr, "%*[^\n]\n");
    }

    for (int k=0; k<num_links; k++) {
        fscanf(fr, "%d %d", &from, &to); // need fromNode only to check if self-link
        if (from != to) { // omit self-links
            counter[to] += 1;
        }
    }

    for (int i=0; i<(*N); i++) {
        (*row_ptr)[i+1] += (*row_ptr)[i] + counter[i];
    }

    /*for (int i=0; i<(*N); i++) {
        printf("Row_ptr[%d] = %d\n", i+1, (*row_ptr)[i+1]);
    }
    for (int j=0; j<num_edges; j++) {
        printf("Col_idx[%d] = %d\n", j, (*col_idx)[j]);
        printf("Val[%d] = %lf\n", j, (*val)[j]);
    }*/
}