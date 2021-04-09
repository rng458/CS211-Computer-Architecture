#include<stdio.h>
#include<stdlib.h>

int** create_mainematrix(FILE*, int);
int** create_smallermatrix(int**, int, int);
int find_determinant(int**, int, int);
void free_matrix(int**, int);

int** create_mainmatrix(FILE* mat1, int dim) {
  int** nMatrix = malloc(dim * sizeof(int*));
  for(int i = 0; i < dim; i++) {
    nMatrix[i] = malloc(dim * sizeof(int));
  }

  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < dim; j++) {
      int num;
      fscanf(mat1, "%d\n", &num);
      nMatrix[i][j] = num;
    }
  }
  
  return  nMatrix;
}

int** create_smallermatrix(int** ogM, int dim, int col) {
  int** nMatrix;
  nMatrix = malloc(dim * sizeof(int*));
  for(int i = 0; i < dim; i++) {
    nMatrix[i] = malloc(dim * sizeof(int));
  }

  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < dim; j++) {
      if(j >= col) {
	nMatrix[i][j] = ogM[i+1][j+1];
      }
      else {
	nMatrix[i][j] = ogM[i+1][j];
      }
    }
  }
  return nMatrix;
}

int find_determinant(int** mat, int dim, int col) {

  if(dim <= 2) {
    int det;
    det = ((mat[0][0])*(mat[1][1])) - ((mat[0][1])*(mat[1][0]));
    return det;
  }
  int sum = 0;
  int sign1 = -1;
  int sign2 = -1;
  for(int i = 0; i < dim; i++) {
    int** nMatrix = create_smallermatrix(mat, dim-1, i);
    sign2 = sign1*sign2;
    sum = sum + ((sign2)*(mat[0][i]*find_determinant(nMatrix, dim-1, i)));
    free_matrix(nMatrix, dim-1);
  }
  
  return sum;
}

void free_matrix(int** mat, int dim) {
  for(int i = 0; i < dim; i++) {
    free(mat[i]);
  }
  free(mat);
}

int main(int argc, char* argv[argc+1]) {

  FILE* mat1 = fopen(argv[1], "r");

  if(mat1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int dimension;
  fscanf(mat1, "%d\n", &dimension);

  int** mainmatrix;
  mainmatrix = create_mainmatrix(mat1, dimension);
  int result = find_determinant(mainmatrix, dimension, 0);

  printf("%d\n", result);
  
  free_matrix(mainmatrix, dimension);
  fclose(mat1);
  return EXIT_SUCCESS;
}
