#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[argc+1]){

  FILE* intlist = fopen(argv[1], "r");

  int primer;

  while(fscanf(intlist, "%d\n", &primer) != EOF) {

    if(primer < 3) {
      printf("no\n");
    }
    else {
      int prime = 1;
      int twinprime = 1;

      //check if current number is prime
      for(int i = 2; i <= (primer/2); i++) {
	if(primer%i == 0) {
	  prime = 0;
	  printf("no\n");
	  break;
	}
      }

      //if so, we have to see if it has a twin
      if(prime) {
	int uprimer = primer+2;
	int lprimer = primer-2;

	for(int i = 2; i <= (uprimer/2); i++) {
	  if(uprimer%i == 0) {
	    twinprime = 0;
	    break;
	  }
	}
      
	if(twinprime) {
	  printf("yes\n");
	}

	if(twinprime == 0) {
	  //reset twinprime
	  twinprime = 1;
	  for(int i = 2; i <= (lprimer/2); i++) {
	    if(lprimer%i == 0) {
	      twinprime = 0;
	      break;
	    }
	  }

	  if(twinprime) {
	    printf("yes\n");
	  } else {
	    printf("no\n");
	  }
	}
      }
    }
  }
  
  fclose(intlist);
  return EXIT_SUCCESS;
}
