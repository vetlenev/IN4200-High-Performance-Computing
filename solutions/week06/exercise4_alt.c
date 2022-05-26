/*
Suppose there is a 2D rectangular uniform mesh, which has M points in the
x-direction and N points in the y-direction. Please describe in detail how to
carry out a checkerboard block decomposition of the mesh into S ×T blocks,
as evenly as possible. Note that M may not be divisible by S, and N may
not be divisible by T
*/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/* Struct Matrix
 * The Matrix struct is an example of a possible mesh,
 * containing the variables necessary to define its dimensions (M, N);
 *
 * S, T are technically used for parallelization purposes.
 */
struct Matrix {
    int M, N; // matrix dimensions
    int S, T; // multiproc grid
    float * data; // M * N
};

// WORKLOAD IS ONLY CALLED IN PARALLEL
/* workload
 * - struct Matrix *mesh: pointer to struct of type Matrix holding
 *                        a theoretical matrix for compuation purposes.
 * - int x: an iteration value, used to find the identity of the thread
 *          in both dimension x (used by M and S) and y (used by N and T).
 */
int workload(struct Matrix *mesh, int x) {
  int i = x / mesh->S,
      j = x % mesh->T;

  // S_i x T_j
  int k, l;
  // k and l are the size of the grid this specific thread is to work on

  k = (mesh->M / mesh->S) + ((i < (mesh->M % mesh->S)) ? 1 : 0);
  l = (mesh->N / mesh->T) + ((j < (mesh->N % mesh->T)) ? 1 : 0);

  /* The above lines are synonymous with the below example:
   * > k = (mesh->M / mesh->S);
   * > if (i < (mesh->M % mesh->S)) k++;
   */



  // Where to start our workload:
  int k_sta, l_sta;

  // naive assumption of starting coordinates
  k_sta = (mesh->M / mesh->S) * i;
  l_sta = (mesh->N / mesh->T) * j;

  k_sta+= i<(mesh->M % mesh->S) ? i : (mesh->M % mesh->S);
  l_sta+= j<(mesh->N % mesh->T) ? j : (mesh->N % mesh->T);

  // Have thread identify itself, and the size region it is working on, and where said region starts
  printf("Thread[%d, %d] reporting for duty at [%d, %d] with size %d x %d\n", i, j, k_sta, l_sta, k, l);

  /* WORKLOAD */

  return 0;
}

int main(int argc, char const *argv[]) {
  Matrix *mesh = (Matrix *) malloc (sizeof(Matrix));
  mesh->M = 10;
  mesh->N = 10;
  mesh->S = 3;
  mesh->T = 3;
  // mesh->data = (float *) malloc (sizeof(float) * mesh->M * mesh->N)

  printf("Space: [%d, %d], Threads: [%d, %d]\n", mesh->M, mesh->N, mesh->S, mesh->T);

  // distribute iterations of loop across cores/threads
  #pragma omp parallel for
  for (size_t x = 0; x < mesh->S * mesh->T; x++) { // 9 workload calls
    workload(mesh, x);
    // 0, 1, 2, 3
    // 4, 5, 6, 7
    // 8
  }
  return 0;
}
