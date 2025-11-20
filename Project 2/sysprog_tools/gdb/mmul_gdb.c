#include <stdio.h>
#include <stdlib.h>

double ** alloc_matrix(int size);
void free_matrix(double ** M, int size);
void matrix_rand_init(double ** M, int size);
/*void print_matrix(double **M, int size);*/


int main(int argc, char** argv){

  if (argc != 2) {
    fprintf(stdout,"Usage: %s #size\n",argv[0]);
    return 1;
  }

  int size = atoi(argv[1]);

  /* allocate matrices spaces */
  double ** A = alloc_matrix(size);
  double ** B = alloc_matrix(size);
  double ** C = alloc_matrix(size/2);

  /* initialize matrices */
  matrix_rand_init(A, size);
  matrix_rand_init(B, size);

  int i,j,k;

  /****** this is where your computation goes *********/
  for (i=0; i<size; i++) {
    for (j=0; j<size; j++) {
      double tmp = 0;
      for (k=0; k<size; k++) {
          tmp += A[i][k]*B[k][j];
      }
      C[i][j] = tmp;
    }
  }

  /* free matrices */
  free_matrix(A,size);
  free_matrix(B,size);
  free_matrix(C,size);

  exit(0);
}

double ** alloc_matrix(int size) {
  int i;
  double ** M = malloc(size * sizeof(double *));
  for (i=0; i<size; i++) {
    M[i] = calloc(1, size * sizeof(double));
  }
  return M;
}

void free_matrix(double ** M, int size) {
  int i;
  for (i=0; i<size; i++) {
    free(M[i]);
  }
  free(M);
}

void matrix_rand_init(double ** M, int size) {
  int i,j;
  for (i=0; i<size; i++) {
    for (j=0; j<size; j++) {
      M[i][j] = rand()/123.0;
    }
  }
}

void print_matrix(double **M, int size) {
  int i, j;
  for (i=0; i<size; i++) {
    for (j=0; j<size; j++)
      printf("%f\t", M[i][j]);
    printf("\n");
  }
}
