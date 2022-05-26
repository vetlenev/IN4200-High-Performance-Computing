#include <stdlib.h>

double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

int *block2D(int k, int M, int N, int P, int Q) 
{
    /* 
    2D block decomposition of M x N grid into
    P x Q blocks (i.e P rowwise blocks and Q colwise blocks)
    */
    int *bounds = malloc(4*sizeof(int));
    int x_start = (k/Q)*(M/P) + ((P - k/Q < M % P)?(M%P-(P-k/Q)):0);
    int x_stop = (k/Q + 1)*(M/P) + ((P - k/Q <= M % P)?(M%P-(P-k/Q)+1):0);
    int y_start = (k%Q)*(N/Q) + ((k%Q > (Q - N%Q))?(k%Q-(Q-N%Q)):0);
    int y_stop = (k%Q + 1)*(N/Q) + ((k%Q >= (Q - N%Q))?(k%Q-(Q-N%Q)+1):0);

    bounds[0] = x_start; bounds[1] = x_stop;
    bounds[2] = y_start; bounds[3] = y_stop;

    return bounds;
}

void read_matrix_binaryformat(char* filename, double*** matrix, int* num_rows, int* num_cols)
{
    int i;
    FILE* fp = fopen (filename,"rb");
    fread (num_rows, sizeof(int), 1, fp);
    fread (num_cols, sizeof(int), 1, fp);

    /* storage allocation of the matrix */
    *matrix = (double**)malloc((*num_rows)*sizeof(double*));
    (*matrix)[0] = (double*)malloc((*num_rows)*(*num_cols)*sizeof(double));
    for (i=1; i<(*num_rows); i++)
        (*matrix)[i] = (*matrix)[i-1]+(*num_cols);

    /* read in the entire matrix */
    fread ((*matrix)[0], sizeof(double), (*num_rows)*(*num_cols), fp);
    fclose (fp);
}

void write_matrix_binaryformat (char* filename, double** matrix, int num_rows, int num_cols)
{
    FILE *fp = fopen (filename,"wb");
    fwrite (&num_rows, sizeof(int), 1, fp);
    fwrite (&num_cols, sizeof(int), 1, fp);
    fwrite (matrix[0], sizeof(double), num_rows*num_cols, fp);
    fclose (fp);
}
