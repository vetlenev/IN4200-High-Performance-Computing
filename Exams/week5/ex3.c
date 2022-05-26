// UNOPTIMIZED:
void foo (int N, double **mat, double **s, int *v) {
    int i,j;
    double val;
    for (j=0; j<N; j++)
        for (i=0; i<N; i++) {
            val = 1.0*(v[j]%256);
            mat[i][j] = s[i][j]*(sin(val)*sin(val)-cos(val)*cos(val));
    }
}

// OPTIMIZED
void foo_optimized (int N, double **mat, double **s, int *v) {
    int i,j;
    double val;
    double sin_cos_term;

    for (i=0; i<N; i++)
        for (j=0; j<N; j++) {
            val = 1.0*(v[i]%256);
            sin_cos_term = -cos(2*val);
            mat[i][j] = s[i][j]*sin_cos_term;
    }
}

// FASTEST
void foo_optimized (int N, double **mat, double **s, int *v) {
    int i,j;
    double sin_cos_term;

    double *val = malloc(256 * sizeof *val);
    int *vtab = malloc(N * sizeof *vtab);

    for (i=0; i<256; i++) {
        val[i] = -cos(2.0*i);
    }

    for (j=0; j<N; j++) {
        vtab[j] = v[j]%256;
    }

    for (i=0; i<N; i++)
        for (j=0; j<N; j++) {
            mat[i][j] = s[i][j]*val[vtab[i]];
            // Since vtab[i] is a value between 0 and 255, we are sure that indexing val with vtab aquires valid elements
    }
}
