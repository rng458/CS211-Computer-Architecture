#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct cacheBlock {
  int valid;
  unsigned long long accessID;
  unsigned long long tag;
  unsigned long long setInd;
  unsigned long long blockInd;
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

  unsigned long long cSize = atoi(argv[1]);
  char* cPolicy = argv[3];
  unsigned long long cBlockSize = atoi(argv[4]);
  unsigned long long cAssociativity = determineAssoc(argv[2], cSize, cBlockSize);
  FILE* trace = fopen(argv[5], "r");

  //to check for valid inputs
  //enough arguments?
  if(argc != 6) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  //powers of 2 test
  int pwr2 = 2;
  int cSizepwr2 = 0;
  int cBlockSizepwr2 = 0;
  while(pwr2 <= cSize || pwr2 <= cBlockSize) {
    if(pwr2 == cSize) {
      cSizepwr2 = 1;
    }
    if(pwr2 == cBlockSize) {
      cBlockSizepwr2 = 1;
    }
    pwr2 = pwr2*2;
  }
  if(!cSizepwr2 || !cBlockSizepwr2) {
    printf("error\n");
    return EXIT_SUCCESS;
  }
  //valid file?
  if(!trace) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  //malloc a 2d array
  unsigned long long amountOfSets = cSize/(cAssociativity*cBlockSize);
  struct cacheBlock** l1cache = malloc(amountOfSets*sizeof(struct cacheBlock*));
  for(int i = 0; i < amountOfSets; i++) {
    l1cache[i] = malloc(cAssociativity*sizeof(struct cacheBlock));
    for(int j = 0; j < cAssociativity; j++) {
      l1cache[i][j].valid = 0;
    }
  }

  //determining bit lengths
  int amountOfBlockBits = 0;
  int amountOfSetBits = 0;
  int blockCounter = cBlockSize;
  int setCounter = amountOfSets;

  while(blockCounter % 2 == 0) {
    amountOfBlockBits++;
    blockCounter = blockCounter/2;
  }
  while(setCounter % 2 == 0) {
    amountOfSetBits++;
    setCounter = setCounter/2;
  }
  
  int amountOfTagBits = 48 - (amountOfBlockBits+amountOfSetBits);
  
  //for replacement policy
  unsigned long long positionCounter = 0;
  
  //for return values
  unsigned long long memread = 0;
  unsigned long long memwrite = 0;
  unsigned long long cachehit = 0;
  unsigned long long cachemiss = 0;
  
  //scan file
  char instruction;
  unsigned long long address;
  while(fscanf(trace,"%c %llx\n", &instruction, &address) != EOF) {
    //masking each part
    unsigned long long tagA = ((address>>(amountOfBlockBits + amountOfSetBits)) & ((int)pow(2,amountOfTagBits)-1))<<(amountOfBlockBits + amountOfSetBits);
    unsigned long long setA = (address>>amountOfBlockBits) & ((int)pow(2, amountOfSetBits)-1);
    unsigned long long blockA =(address) & ((int)pow(2, amountOfBlockBits)-1);
    if(instruction == 'W') {
      int alreadyExist = 0;
      for(int i = 0; i < cAssociativity; i++) {
	if((l1cache[setA][i].valid == 1) && (l1cache[setA][i].tag == tagA)) {
	  if(strcmp(cPolicy, "lru") == 0) {
	    l1cache[setA][i].accessID = positionCounter;
	    positionCounter++;
	  }
	  
	  alreadyExist = 1;
	  cachehit++;
	  memwrite++;
	  break;
	}
      }

      if(!alreadyExist) {
	int availableSpace = 0;
	for(int i = 0; i < cAssociativity; i++) {
	  if(l1cache[setA][i].valid == 0) {
	    l1cache[setA][i].valid = 1;
	    l1cache[setA][i].accessID = positionCounter;
	    positionCounter++;
	    l1cache[setA][i].tag = tagA;
	    l1cache[setA][i].setInd = setA;
	    l1cache[setA][i].blockInd = blockA;
	    
	    availableSpace = 1;
	    cachemiss++;
	    memread++;
	    memwrite++;
	    break;
	  }
	}

	if(!availableSpace) {
	  int targetInd = 0;
	  for(int i = 0; i < cAssociativity; i++) {
	    if(l1cache[setA][targetInd].accessID > l1cache[setA][i].accessID) {
	      targetInd = i;
	    }
	  }
	  l1cache[setA][targetInd].accessID = positionCounter;
	  positionCounter++;
	  l1cache[setA][targetInd].tag = tagA;
	  l1cache[setA][targetInd].setInd = setA;
	  l1cache[setA][targetInd].blockInd = blockA;

	  cachemiss++;
	  memread++;
	  memwrite++;
	}
      }
    }
    else if(instruction == 'R') {
      int alreadyExist = 0;
      for(int i = 0; i < cAssociativity; i++) {
	if((l1cache[setA][i].valid == 1) && (l1cache[setA][i].tag == tagA)) {
	  if(strcmp(cPolicy, "lru") == 0) {
	    l1cache[setA][i].accessID = positionCounter;
	    positionCounter++;
	  }
	  
	  alreadyExist = 1;
	  cachehit++;
	  break;
	}
      }

      if(!alreadyExist) {
	int availableSpace = 0;
	for(int i = 0; i < cAssociativity; i++) {
	  if(l1cache[setA][i].valid == 0) {
	    l1cache[setA][i].valid = 1;
	    l1cache[setA][i].accessID = positionCounter;
	    positionCounter++;
	    l1cache[setA][i].tag = tagA;
	    l1cache[setA][i].setInd = setA;
	    l1cache[setA][i].blockInd = blockA;
	    
	    availableSpace = 1;
	    cachemiss++;
	    memread++;
	    break;
	  }
	}

	if(!availableSpace) {
	  int targetInd = 0;
	  for(int i = 0; i < cAssociativity; i++) {
	    if(l1cache[setA][targetInd].accessID > l1cache[setA][i].accessID) {
	      targetInd = i;
	    }
	  }
	  l1cache[setA][targetInd].accessID = positionCounter;
	  positionCounter++;
	  l1cache[setA][targetInd].tag = tagA;
	  l1cache[setA][targetInd].setInd = setA;
	  l1cache[setA][targetInd].blockInd = blockA;

	  cachemiss++;
	  memread++;
	}
      }
    }
  }

  printf("memread:%llu\n", memread);
  printf("memwrite:%llu\n", memwrite);
  printf("cachehit:%llu\n", cachehit);
  printf("cachemiss:%llu\n", cachemiss);

  freeCache(l1cache, amountOfSets, cAssociativity);

  /*
  //debug
  printf("Here are your inputs: \n");
  printf("Cache size: %llu\n", cSize);
  printf("Cache Associativity: %llu\n", cAssociativity);
  printf("Cache Policy: %s\n", cPolicy);
  printf("Cache Block Size: %llu\n", cBlockSize);
  printf("Trace file name: %s\n", argv[5]);
  */
  return EXIT_SUCCESS;
}
