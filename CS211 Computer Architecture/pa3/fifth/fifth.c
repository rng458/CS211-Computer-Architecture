#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printBinary(long v, int length) {
  for(int i = length-1; i >= 0; i--) {
    if((v>>i)&1) {
      printf("1");
    }
    else {
      printf("0");
    }
  }
}

void debugPrint(double number, int bias, int exponent, int sign, int exp, double mantissa) {
  printf("number: %lf\n", number);
  printf("bias: %d\n", bias);
  printf("exponent: %d\n", exponent);
  printf("sign: %d\n", sign);
  printf("exp: %d\n", exp);
  printf("mantissa: %lf\n", mantissa);
  printf("----------------------\n");
}

int main(int argc, char* argv[argc+1]) {

  FILE* f1 = fopen(argv[1], "r");
  if(f1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  double origNum;
  int totalBits, expBits, mantBits;
  while(fscanf(f1, "%lf %d %d %d\n", &origNum, &totalBits, &expBits, &mantBits) != EOF) {
    int bias = (pow(2, expBits-1))-1;
    int sign;
    if(origNum < 0) {
      sign = 1;
    }
    else {
      sign = 0;
    }

    int exponent = 0;
    double absNum = fabs(origNum);
    while(absNum > 2) {
      absNum = absNum/2;
      exponent++;
    }
    while(absNum < 1) {
      absNum = absNum*2;
      exponent--;
    }
    double mantissa = fmod(absNum, 1);
    int exp = exponent+bias;

    if(exponent < 1-bias) { //denormal
      while(absNum > 1) {
	absNum = absNum/2;
	exponent++;
      }
      exp = 0;
      mantissa = fmod(absNum, 1);

      long mantissaB = 0;
      double mantissaTemp = mantissa;
      for(int i = mantBits+2; i >= 0; i--) { //this puts the mantissa into binary
        mantissaTemp = mantissaTemp*2;
	if(mantissaTemp >= 1) {
	  mantissaB = ((mantissaB>>i)+1)<<i;
	}
	mantissaTemp = fmod(mantissaTemp, 1);
      }
      int g = (mantissaB&(1<<2))>>2;
      int r = (mantissaB&(1<<1))>>1;
      int s = mantissaB&1;
      long mask = pow(2, mantBits)-1;
      long roundDown = (mantissaB&(mask<<3))>>3;
      long roundUp = roundDown+1;

      if(!g) {
	printf("%d", sign);
	printBinary(exp, expBits);
	printBinary(roundDown, mantBits);
      }
      else if(g && !r && !s) {
	if(!(roundDown&1)) {
	  printf("%d", sign);
	  printBinary(exp, expBits);
	  printBinary(roundDown, mantBits);
	}
	else if(!(roundUp&1)) {
	  printf("%d", sign);
	  printBinary(exp, expBits);
	  printBinary(roundUp, mantBits);
	}
      }
      else if(g && (r || s)) {
	printf("%d", sign);
	printBinary(exp, expBits);
	printBinary(roundUp, mantBits);
      }
      printf("\n");
    }
    else { //normal
      long mantissaB = 0;
      double mantissaTemp = mantissa;
      for(int i = mantBits+2; i >= 0; i--) { //this puts the mantissa into binary
        mantissaTemp = mantissaTemp*2;
	if(mantissaTemp >= 1) {
	  mantissaB = ((mantissaB>>i)+1)<<i;
	}
	mantissaTemp = fmod(mantissaTemp, 1);
      }

      int g = (mantissaB&(1<<2))>>2;
      int r = (mantissaB&(1<<1))>>1;
      int s = mantissaB&1;
      long mask = pow(2, mantBits)-1;
      long roundDown = (mantissaB&(mask<<3))>>3;
      long roundUp = roundDown+1;

      if(!g) {
	printf("%d", sign);
	printBinary(exp, expBits);
	printBinary(roundDown, mantBits);
      }
      else if(g && !r && !s) {
	if((roundUp > mask)) {
	    if(exponent%2) {
	      exp++;
	      printf("%d", sign);
	      printBinary(exp, expBits);
	      printBinary(roundUp, mantBits);
	    }
	    else {
	      printf("%d", sign);
	      printBinary(exp, expBits);
	      printBinary(roundDown, mantBits);
	    }
	}
	else {
	  if(!(roundDown&1)) {
	    printf("%d", sign);
	    printBinary(exp, expBits);
	    printBinary(roundDown, mantBits);
	  }
	  else if(!(roundUp&1)) {
	    printf("%d", sign);
	    printBinary(exp, expBits);
	    printBinary(roundUp, mantBits);
	  }
	}
      }
      else if(g && (r || s)) {
	printf("%d", sign);
	printBinary(exp, expBits);
	printBinary(roundUp, mantBits);
      }
      printf("\n");
    }
    //debugPrint(absNum, bias, exponent, sign, exp, mantissa);
  }
  
  fclose(f1);
  return EXIT_SUCCESS;
}
