#include<stdio.h>
#include<stdlib.h>

int** create_matrix(int n) {
  int** magic_square = malloc(n*sizeof(int*));
  for(int i = 0; i < n; i++) {
    magic_square[i] = malloc(n*sizeof(int));
  }

  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      magic_square[i][j] = 0;
    }
  }
  
  return magic_square;
}

void print_matrix(int** matrix, int n) {
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
}

void free_matrix(int** matrix, int n) {
  for(int i = 0; i < n; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

int main(int argc, char* argv[argc+1]) {

  if(argc < 2) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  
  int n = atoi(argv[1]);

  if((n%2 == 0)) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int max = n*n;
  int magR = 0;
  int magC = (n-1)/2;
  int** magic_square = create_matrix(n);

  magic_square[magR][magC] = 1;
  for(int num = 2; num <= max; num++) {
    int orgR = magR;
    int orgC = magC;
    magR = magR-1;
    magC = magC+1;

    if(magR == -1) {
      magR = n-1;
    }
    if(magC == n) {
      magC = 0;
    }
    
    if(magic_square[magR][magC] == 0) {
      magic_square[magR][magC] = num;
    }
    else {
      magR = orgR + 1;
      magC = orgC;

      if(magR == n) {
	magR = 0;
      }
      
      magic_square[magR][magC] = num;
    }
  }

  print_matrix(magic_square, n);
  free_matrix(magic_square, n);
  return EXIT_SUCCESS;
}
