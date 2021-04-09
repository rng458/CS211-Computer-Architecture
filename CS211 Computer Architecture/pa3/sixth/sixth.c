#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printBinary(long v, int length) {
  for(int i = length-1; i >= 0; i--) {
    if((v>>i) & 1) {
      printf("1");
    }
    else {
      printf("0");
    }
  }
  printf("\n");
}

void debugPrint(long number, int normal, int bias, int sign, int exp, int mantissa) {
  printf("number: %ld or 0x%lx\n", number, number);
  if(normal) {
    printf("normal: yes\n");
  }
  else {
    printf("normal: no\n");
  }
  printf("bias: %d\n", bias);
  printf("sign: %d\n", sign);
  printf("exp: %d\n", exp);
  printf("mantissa: %d\n", mantissa);
  printf("-------------------------------\n");
}

int main(int argc, char* argv[argc+1]) {

  FILE* f1 = fopen(argv[1], "r");
  if(f1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  long num;
  int totalBits, expBits, mantBits, precision;
  while(fscanf(f1, "%d %d %d %lx %d\n", &totalBits, &expBits, &mantBits, &num, &precision) != EOF) {
    long signMask = pow(2, totalBits-(expBits+mantBits))-1;
    long expMask = pow(2, expBits)-1;
    long mantissaMask = pow(2, mantBits)-1;
    int sign = (num & (signMask<<(expBits+mantBits)))>>(expBits+mantBits);
    int exp = (num & (expMask<<mantBits))>>mantBits;
    int mantissa = num & mantissaMask;
    int bias = pow(2, expBits-1)-1;
    int normal;
    int exponent;

    if(exp > 0) {
      normal = 1;
      exponent = exp-bias;
    }
    else {
      normal = 0;
      exponent = 1-bias;
    }

    if(normal) {
      double tmpValue = 1;
      double mantTotal = 0;
      for(int i = mantBits-1; i >= 0; i--) {
	tmpValue = tmpValue/2;
	if((mantissa>>i) & 1) {
	  mantTotal += tmpValue;
	}
      }
      double result = 1+mantTotal;
      result = result * pow(-1, sign) * pow(2, exponent);
      printf("%.*lf\n", precision, result);
    }
    else {
      double tmpValue = 1;
      double mantTotal = 0;
      for(int i = mantBits-1; i >= 0; i--) {
	tmpValue = tmpValue/2;
	if((mantissa>>i) & 1) {
	  mantTotal += tmpValue;
	}
      }
      double result = mantTotal;
      result = result * pow(-1, sign) * pow(2, exponent);
      printf("%.*lf\n", precision, result);
    }

    //debugPrint(num, normal, bias, sign, exp, mantissa);
  }
  
  fclose(f1);
  return EXIT_SUCCESS;
}
