#include <malloc.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    int M, T;
    if (argc < 2) {
        M = 42;
    } else {
        M = atoi(argv[1]);
    }
    if (argc < 3) {
        T = 42;
    } else {
        T = atoi(argv[2]);
    }

    double x, dx = 1.0/(M+1);
    double t, dt = 1. / (T-1);
    double *tmp;
    int i;

    /* allocating three 1D arrays um, u, up of length M+2 */
    /* ... */
    double *um = malloc(sizeof *um * (M+2));
    double *u = malloc(sizeof *u * (M+2));
    double *up = malloc(sizeof *up * (M+2));

    for (i=0; i<=M+1; i++) {
        x = i*dx;
        um[i] = x > .5 ? 1. : -1.; // Initial values
    }

    for (i=1; i<=M; i++)
        u[i] = um[i] + 0.5*(um[i-1]-2*um[i]+um[i+1]);

    u[0] = u[M+1] = 0.0;

    t = dt;
    while (t<1.0) {
        t += dt;
        for (i=1; i<=M; i++)
            up[i] = (um[i]+u[i-1]+u[i+1]) / 3;
        up[0] = up[M+1] = 0.0;

        /* shuffle the three arrays */
        tmp = um;
        um = u;
        u = up;
        up = tmp;
    }

    // Output for plotting
    FILE *fp = fopen("w11_e3.dat", "w");
    for (size_t i = 0; i < M + 2; i++) {
        fprintf(fp, "%.17g, %.17g\n", (double)i/(M+1),
                                      up[i]);
    }
    fclose(fp);

    free(um);
    free(u);
    free(up);
}
