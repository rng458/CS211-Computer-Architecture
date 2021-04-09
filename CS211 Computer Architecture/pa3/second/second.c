#include <stdio.h>
#include <stdlib.h>

void printBits(int w, long v) {
  for(int i = w-1; i >= 0; i--) {
	long comp = v>>i;
	if(comp & 1) {
	  printf("1");
	}
	else {
	  printf("0");
	}
      }
      printf("\n");
}

int enoughBits(int w, long v) {
  long maxVP = 0;
  long maxVN = 0;

  for(int i = w-1; i > 0; i--) {
    if(maxVP == 0) {
      maxVP = 2;
    }
    else {
      maxVP = maxVP*2;
    }
  }
  
  if(maxVP) {
    maxVP = (maxVP)-1;
  }
  maxVN = (-1)*maxVP-1;

  if(v < maxVN) {
    printBits(w, maxVN);
    return 0;
  }
  else if(v > maxVP) {
    printBits(w, maxVP);
    return 0;
  }

  return 1;
}

int main(int argc, char* argv[argc+1]) {

  FILE* f1 = fopen(argv[1], "r");
  if(f1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  long num = 0;
  int w = 0;

  while(fscanf(f1, "%ld %d\n", &num, &w) != EOF) {
    if(enoughBits(w, num)) {
      printBits(w, num);
    }
  }

  fclose(f1);
  return EXIT_SUCCESS;
}
