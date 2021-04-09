#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[argc+1]) {

  FILE* f1 = fopen(argv[1], "r");
  if(f1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  double decnum;
  int bits;

  while(fscanf(f1, "%lf %d\n", &decnum, &bits) != EOF) {
    double translatednum = decnum;
    int exponent = 0;
    if(translatednum > 2) {
      while(translatednum > 2) {
	translatednum = translatednum/2;
	exponent++;
      }
    }
    else if(translatednum < 1) {
      while(translatednum < 1) {
	translatednum = translatednum*2;
	exponent--;
      }
    }

    printf("1.");
    double decimal = translatednum-1;
    for(int i = 0; i < bits; i++) {
      decimal = decimal*2;
      if(decimal >= 1) {
	printf("1");
      }
      else if(decimal < 1) {
	printf("0");
      }
      decimal = fmod(decimal, 1);
    }
    printf(" %d\n", exponent);
    
  }

  fclose(f1);
  return EXIT_SUCCESS;
}
