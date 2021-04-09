#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct cacheBlock {
  int valid;
  unsigned long long l1accessID;
  unsigned long long l2accessID;
  unsigned long long l1tag;
  unsigned long long l1setInd;
  unsigned long long l1blockInd;
  unsigned long long l2tag;
  unsigned long long l2setInd;
  unsigned long long l2blockInd;
};

void freeCache(struct cacheBlock** cache, unsigned long long amountOfSets, unsigned long long associativity) {
  for(int i = 0; i < amountOfSets; i++) {
    free(cache[i]);
  }
  free(cache);
}

unsigned long long determineAssoc(char* input, unsigned long long cacheSize, unsigned long long blockSize) {
  if(strcmp("direct", input) == 0) {
    return 1;
  }
  if(strcmp("assoc", input) == 0) {
    return cacheSize/blockSize;
  }
  if(input[5] == ':') {
    char* n = &input[6];
    return atoi(n);
  }
  return 0;
}

int main(int argc, char* argv[argc+1]) {

  //level 1 cache variables
  unsigned long long l1cSize = atoi(argv[1]);
  char* l1cPolicy = argv[3];
  unsigned long long l1cBlockSize = atoi(argv[4]);
  unsigned long long l1cAssociativity = determineAssoc(argv[2], l1cSize, l1cBlockSize);

  //level 2 cache variables
  unsigned long long l2cSize = atoi(argv[5]);
  //char* l2cPolicy = argv[7];
  unsigned long long l2cBlockSize = l1cBlockSize;
  unsigned long long l2cAssociativity = determineAssoc(argv[6], l2cSize, l2cBlockSize);
  
  FILE* trace = fopen(argv[8], "r");

  //check for valid inputs
  //enough arguments?
  if(argc != 9) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  //powers of 2 test
  int pwr2 = 2;
  int l1cSizepwr2 = 0;
  int l1cBlockSizepwr2 = 0;
  int l2cSizepwr2 = 0;
  int l2cBlockSizepwr2 = 0;
  while(pwr2 <= l1cSize || pwr2 <= l1cBlockSize || pwr2 <= l2cSize || pwr2 <= l2cBlockSize) {
    if(pwr2 == l1cSize) {
      l1cSizepwr2 = 1;
    }
    if(pwr2 == l1cBlockSize) {
      l1cBlockSizepwr2 = 1;
    }
    if(pwr2 == l2cSize) {
      l2cSizepwr2 = 1;
    }
    if(pwr2 == l2cBlockSize) {
      l2cBlockSizepwr2 = 1;
    }
    pwr2 = pwr2*2;
  }
  if(!l1cSizepwr2 || !l1cBlockSizepwr2 || !l2cSizepwr2 || !l2cBlockSizepwr2) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  //valid file?
  if(!trace) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  //make l1 cache 2d array
  unsigned long long l1amountOfSets = l1cSize/(l1cAssociativity*l1cBlockSize);
  struct cacheBlock** l1cache = malloc(l1amountOfSets*sizeof(struct cacheBlock*));
  for(int i = 0; i < l1amountOfSets; i++) {
    l1cache[i] = malloc(l1cAssociativity*sizeof(struct cacheBlock));
    for(int j = 0; j < l1cAssociativity; j++) {
      l1cache[i][j].valid = 0;
    }
  }
  //make l2 cache 2d array
  unsigned long long l2amountOfSets = l2cSize/(l2cAssociativity*l2cBlockSize);
  struct cacheBlock** l2cache = malloc(l2amountOfSets*sizeof(struct cacheBlock*));
  for(int i = 0; i < l2amountOfSets; i++) {
    l2cache[i] = malloc(l2cAssociativity*sizeof(struct cacheBlock));
    for(int j = 0; j < l2cAssociativity; j++) {
      l2cache[i][j].valid = 0;
    }
  }

  //l1 cache bit lengths
  int l1amountOfBlockBits = 0;
  int l1amountOfSetBits = 0;
  int l1blockCounter = l1cBlockSize;
  int l1setCounter = l1amountOfSets;

  while(l1blockCounter % 2 == 0) {
    l1amountOfBlockBits++;
    l1blockCounter = l1blockCounter/2;
  }
  while(l1setCounter % 2 == 0) {
    l1amountOfSetBits++;
    l1setCounter = l1setCounter/2;
  }

  int l1amountOfTagBits = 48 - (l1amountOfBlockBits + l1amountOfSetBits);
  //l2 cache bit lengths
  int l2amountOfBlockBits = 0;
  int l2amountOfSetBits = 0;
  int l2blockCounter = l2cBlockSize;
  int l2setCounter = l2amountOfSets;

  while(l2blockCounter % 2 == 0) {
    l2amountOfBlockBits++;
    l2blockCounter = l2blockCounter/2;
  }
  while(l2setCounter % 2 == 0) {
    l2amountOfSetBits++;
    l2setCounter = l2setCounter/2;
  }

  int l2amountOfTagBits = 48 - (l2amountOfBlockBits + l2amountOfSetBits);

  //for replacement policy
  unsigned long long l1counter = 0;
  unsigned long long l2counter = 0;

  //for return values
  unsigned long long memread = 0;
  unsigned long long memwrite = 0;
  unsigned long long l1cachehit = 0;
  unsigned long long l1cachemiss = 0;
  unsigned long long l2cachehit = 0;
  unsigned long long l2cachemiss = 0;

  //scan file
  char instruction;
  unsigned long long address;
  while(fscanf(trace, "%c %llx\n", &instruction, &address) != EOF) {
    //masking for l1
    unsigned long long l1tagA = ((address>>(l1amountOfBlockBits + l1amountOfSetBits)) & ((int)pow(2,l1amountOfTagBits)-1))<<(l1amountOfBlockBits + l1amountOfSetBits);
    unsigned long long l1setA = (address>>l1amountOfBlockBits) & ((int)pow(2, l1amountOfSetBits)-1);
    unsigned long long l1blockA =(address) & ((int)pow(2, l1amountOfBlockBits)-1);

    //masking for l2
    unsigned long long l2tagA = ((address>>(l2amountOfBlockBits + l2amountOfSetBits)) & ((int)pow(2,l2amountOfTagBits)-1))<<(l2amountOfBlockBits + l2amountOfSetBits);
    unsigned long long l2setA = (address>>l2amountOfBlockBits) & ((int)pow(2, l2amountOfSetBits)-1);
    unsigned long long l2blockA =(address) & ((int)pow(2, l2amountOfBlockBits)-1);
    
    if(instruction == 'W') {
      int l1alreadyExist = 0;
      for(int i = 0; i < l1cAssociativity; i++) {
	//already exists in l1
	if((l1cache[l1setA][i].valid == 1) && (l1cache[l1setA][i].l1tag == l1tagA)) {
	  if(strcmp(l1cPolicy, "lru") == 0) {
	    l1cache[l1setA][i].l1accessID = l1counter;
	    l1counter++;
	  }
	  
	  l1alreadyExist = 1;
	  l1cachehit++;
	  memwrite++;
	  break;
	}
      }

      int l2alreadyExist = 0;
      if(!l1alreadyExist) {
	for(int i = 0; i < l2cAssociativity; i++) {
	  //exist in l2, not l1
	  if((l2cache[l2setA][i].valid == 1) && (l2cache[l2setA][i].l2tag == l2tagA)) {
	    l2alreadyExist = 1;
	    int l1availableSpace = 0;
	    for(int j = 0; j < l1cAssociativity; j++) {
	      //there is space in this l1 set
	      if(l1cache[l1setA][j].valid == 0) {
		l1cache[l1setA][j] = l2cache[l2setA][i];
		l1cache[l1setA][j].l1accessID = l1counter;
		l1counter++;
		l2cache[l2setA][i].valid = 0;

		l1availableSpace = 1;
		l1cachemiss++;
		l2cachehit++;
		memwrite++;
		break;
	      }
	    }

	    if(!l1availableSpace) {
	      int l1targetInd = 0;
	      for(int j = 0; j < l1cAssociativity; j++) {
		//there is no space in the l1 set
		if(l1cache[l1setA][l1targetInd].l1accessID > l1cache[l1setA][j].l1accessID) {
		  l1targetInd = j;
		}
	      }
	      struct cacheBlock temp = l1cache[l1setA][l1targetInd];
	      l1cache[l1setA][l1targetInd] = l2cache[l2setA][i];
	      l1cache[l1setA][l1targetInd].l1accessID = l1counter;
	      l1counter++;
	      l2cache[l2setA][i].valid = 0;

	      //time to move this l1 block back to your mom's house in l2
	      int l2availableSpace = 0;
	      for(int j = 0; j < l2cAssociativity; j++) {
		if(l2cache[temp.l2setInd][j].valid == 0) {
		  l2cache[temp.l2setInd][j] = temp;
		  l2cache[temp.l2setInd][j].l2accessID = l2counter;
		  l2counter++;

		  l2availableSpace = 1;
		  break;
		}
	      }

	      if(!l2availableSpace) {
		int l2targetInd = 0;
		for(int j = 0; j < l2cAssociativity; j++) {
		  if(l2cache[temp.l2setInd][l2targetInd].l2accessID > l2cache[temp.l2setInd][j].l2accessID) {
		    l2targetInd = j;
		  }
		}
		l2cache[temp.l2setInd][l2targetInd] = temp;
		l2cache[temp.l2setInd][l2targetInd].l2accessID = l2counter;
		l2counter++;
	      }
	      l1cachemiss++;
	      l2cachehit++;
	      memwrite++;
	    }
	  }
	}	
      }

      if(!l2alreadyExist && !l1alreadyExist) {
	int l1availableSpace = 0;
	for(int i = 0; i < l1cAssociativity; i++) {
	  if(l1cache[l1setA][i].valid == 0) {
	    l1cache[l1setA][i].valid = 1;
	    l1cache[l1setA][i].l1accessID = l1counter;
	    l1counter++;
	    l1cache[l1setA][i].l1tag = l1tagA;
	    l1cache[l1setA][i].l1setInd = l1setA;
	    l1cache[l1setA][i].l1blockInd = l1blockA;
	    l1cache[l1setA][i].l2tag = l2tagA;
	    l1cache[l1setA][i].l2setInd = l2setA;
	    l1cache[l1setA][i].l2blockInd = l2blockA;

	    l1availableSpace = 1;
	    l1cachemiss++;
	    l2cachemiss++;
	    memread++;
	    memwrite++;
	    break;
	  }
	}

        if(!l1availableSpace) {
	  int l1targetInd = 0;
	  for(int j = 0; j < l1cAssociativity; j++) {
	    //there is no space in the l1 set
	    if(l1cache[l1setA][l1targetInd].l1accessID > l1cache[l1setA][j].l1accessID) {
	      l1targetInd = j;
	    }
	  }
	  struct cacheBlock temp = l1cache[l1setA][l1targetInd];
	  l1cache[l1setA][l1targetInd].valid = 1;
	  l1cache[l1setA][l1targetInd].l1accessID = l1counter;
	  l1counter++;
	  l1cache[l1setA][l1targetInd].l1tag = l1tagA;
	  l1cache[l1setA][l1targetInd].l1setInd = l1setA;
	  l1cache[l1setA][l1targetInd].l1blockInd = l1blockA;
	  l1cache[l1setA][l1targetInd].l2tag = l2tagA;
	  l1cache[l1setA][l1targetInd].l2setInd = l2setA;
	  l1cache[l1setA][l1targetInd].l2blockInd = l2blockA;

	  //time to move this l1 block back to your mom's house in l2
	  int l2availableSpace = 0;
	  for(int j = 0; j < l2cAssociativity; j++) {
	    if(l2cache[temp.l2setInd][j].valid == 0) {
	      l2cache[temp.l2setInd][j] = temp;
	      l2cache[temp.l2setInd][j].l2accessID = l2counter;
	      l2counter++;

	      l2availableSpace = 1;
	      break;
	    }
	  }

	  if(!l2availableSpace) {
	    int l2targetInd = 0;
	    for(int j = 0; j < l2cAssociativity; j++) {
	      if(l2cache[temp.l2setInd][l2targetInd].l2accessID > l2cache[temp.l2setInd][j].l2accessID) {
		l2targetInd = j;
	      }
	    }
	    l2cache[temp.l2setInd][l2targetInd] = temp;
	    l2cache[temp.l2setInd][l2targetInd].l2accessID = l2counter;
	    l2counter++;	
	  }

	  l1cachemiss++;
	  l2cachemiss++;
	  memread++;
	  memwrite++;
	}
      }

    }
    else if (instruction == 'R') {
      int l1alreadyExist = 0;
      for(int i = 0; i < l1cAssociativity; i++) {
	//already exists in l1
	if((l1cache[l1setA][i].valid == 1) && (l1cache[l1setA][i].l1tag == l1tagA)) {
	  if(strcmp(l1cPolicy, "lru") == 0) {
	    l1cache[l1setA][i].l1accessID = l1counter;
	    l1counter++;
	  }
	  
	  l1alreadyExist = 1;
	  l1cachehit++;
	  break;
	}
      }

      int l2alreadyExist = 0;
      if(!l1alreadyExist) {
	for(int i = 0; i < l2cAssociativity; i++) {
	  //exist in l2, not l1
	  if((l2cache[l2setA][i].valid == 1) && (l2cache[l2setA][i].l2tag == l2tagA)) {
	    l2alreadyExist = 1;
	    int l1availableSpace = 0;
	    for(int j = 0; j < l1cAssociativity; j++) {
	      //there is space in this l1 set
	      if(l1cache[l1setA][j].valid == 0) {
		l1cache[l1setA][j] = l2cache[l2setA][i];
		l1cache[l1setA][j].l1accessID = l1counter;
		l1counter++;
		l2cache[l2setA][i].valid = 0;

		l1availableSpace = 1;
		l1cachemiss++;
		l2cachehit++;
		break;
	      }
	    }

	    if(!l1availableSpace) {
	      int l1targetInd = 0;
	      for(int j = 0; j < l1cAssociativity; j++) {
		//there is no space in the l1 set
		if(l1cache[l1setA][l1targetInd].l1accessID > l1cache[l1setA][j].l1accessID) {
		  l1targetInd = j;
		}
	      }
	      struct cacheBlock temp = l1cache[l1setA][l1targetInd];
	      l1cache[l1setA][l1targetInd] = l2cache[l2setA][i];
	      l1cache[l1setA][l1targetInd].l1accessID = l1counter;
	      l1counter++;
	      l2cache[l2setA][i].valid = 0;

	      //time to move this l1 block back to your mom's house in l2
	      int l2availableSpace = 0;
	      for(int j = 0; j < l2cAssociativity; j++) {
		if(l2cache[temp.l2setInd][j].valid == 0) {
		  l2cache[temp.l2setInd][j] = temp;
		  l2cache[temp.l2setInd][j].l2accessID = l2counter;
		  l2counter++;

		  l2availableSpace = 1;
		  break;
		}
	      }

	      if(!l2availableSpace) {
		int l2targetInd = 0;
		for(int j = 0; j < l2cAssociativity; j++) {
		  if(l2cache[temp.l2setInd][l2targetInd].l2accessID > l2cache[temp.l2setInd][j].l2accessID) {
		    l2targetInd = j;
		  }
		}
		l2cache[temp.l2setInd][l2targetInd] = temp;
		l2cache[temp.l2setInd][l2targetInd].l2accessID = l2counter;
		l2counter++;
	      }

	      l1cachemiss++;
	      l2cachehit++;
	    }
	  }
	}	
      }

      if(!l2alreadyExist && !l1alreadyExist) {
	int l1availableSpace = 0;
	for(int i = 0; i < l1cAssociativity; i++) {
	  if(l1cache[l1setA][i].valid == 0) {
	    l1cache[l1setA][i].valid = 1;
	    l1cache[l1setA][i].l1accessID = l1counter;
	    l1counter++;
	    l1cache[l1setA][i].l1tag = l1tagA;
	    l1cache[l1setA][i].l1setInd = l1setA;
	    l1cache[l1setA][i].l1blockInd = l1blockA;
	    l1cache[l1setA][i].l2tag = l2tagA;
	    l1cache[l1setA][i].l2setInd = l2setA;
	    l1cache[l1setA][i].l2blockInd = l2blockA;

	    l1availableSpace = 1;
	    l1cachemiss++;
	    l2cachemiss++;
	    memread++;
	    break;
	  }
	}

        if(!l1availableSpace) {
	  int l1targetInd = 0;
	  for(int j = 0; j < l1cAssociativity; j++) {
	    //there is no space in the l1 set
	    if(l1cache[l1setA][l1targetInd].l1accessID > l1cache[l1setA][j].l1accessID) {
	      l1targetInd = j;
	    }
	  }
	  struct cacheBlock temp = l1cache[l1setA][l1targetInd];
	  l1cache[l1setA][l1targetInd].valid = 1;
	  l1cache[l1setA][l1targetInd].l1accessID = l1counter;
	  l1counter++;
	  l1cache[l1setA][l1targetInd].l1tag = l1tagA;
	  l1cache[l1setA][l1targetInd].l1setInd = l1setA;
	  l1cache[l1setA][l1targetInd].l1blockInd = l1blockA;
	  l1cache[l1setA][l1targetInd].l2tag = l2tagA;
	  l1cache[l1setA][l1targetInd].l2setInd = l2setA;
	  l1cache[l1setA][l1targetInd].l2blockInd = l2blockA;

	  //time to move this l1 block back to your mom's house in l2
	  int l2availableSpace = 0;
	  for(int j = 0; j < l2cAssociativity; j++) {
	    if(l2cache[temp.l2setInd][j].valid == 0) {
	      l2cache[temp.l2setInd][j] = temp;
	      l2cache[temp.l2setInd][j].l2accessID = l2counter;
	      l2counter++;

	      l2availableSpace = 1;
	      break;
	    }
	  }

	  if(!l2availableSpace) {
	    int l2targetInd = 0;
	    for(int j = 0; j < l2cAssociativity; j++) {
	      if(l2cache[temp.l2setInd][l2targetInd].l2accessID > l2cache[temp.l2setInd][j].l2accessID) {
		l2targetInd = j;
	      }
	    }
	    l2cache[temp.l2setInd][l2targetInd] = temp;
	    l2cache[temp.l2setInd][l2targetInd].l2accessID = l2counter;
	    l2counter++;	
	  }

	  l1cachemiss++;
	  l2cachemiss++;
	  memread++;
	}
      }
    } 
  }

  printf("memread:%llu\n", memread);
  printf("memwrite:%llu\n", memwrite);
  printf("l1cachehit:%llu\n", l1cachehit);
  printf("l1cachemiss:%llu\n", l1cachemiss);
  printf("l2cachehit:%llu\n", l2cachehit);
  printf("l2cachemiss:%llu\n", l2cachemiss);

  freeCache(l1cache, l1amountOfSets, l1cAssociativity);
  freeCache(l2cache, l2amountOfSets, l2cAssociativity);

  /*
  //debug
  printf("Here are your inputs: \n");
  printf("L1 Cache size: %llu\n", l1cSize);
  printf("L1 Cache Associativity: %llu\n", l1cAssociativity);
  printf("L1 Cache Policy: %s\n", l1cPolicy);
  printf("L1 Cache Block Size: %llu\n", l1cBlockSize);
  printf("L2 Cache size: %llu\n", l2cSize);
  printf("L2 Cache Associativity: %llu\n", l2cAssociativity);
  printf("L2 Cache Policy: %s\n", l2cPolicy);
  printf("L2 Cache Block Size: %llu\n", l2cBlockSize);
  printf("Trace file name: %s\n", argv[8]);
  */

  return EXIT_SUCCESS;
}
