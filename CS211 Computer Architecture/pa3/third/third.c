#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[argc+1]) {
  
  FILE* f1 = fopen(argv[1], "r");
  if(f1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  

  int maxbitsize = sizeof(long)*8;
  long num;
  long bits;
  char signB;
  char signA;
  while(fscanf(f1, "%ld %ld %c %c\n", &num, &bits, &signB, &signA) != EOF) {
    if(signB == 'u' && signA == 's') {
      long temp = num<<(maxbitsize-bits);
      temp = temp>>(maxbitsize-bits);
      printf("%ld\n", temp);
    }
    else if(signB == 's' && signA == 'u') {
      unsigned long temp = num<<(maxbitsize-bits);
      temp = temp>>(maxbitsize-bits);
      printf("%lu\n", temp);
    }
  }
  
  fclose(f1);
  return EXIT_SUCCESS;
}
