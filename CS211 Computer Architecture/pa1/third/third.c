#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct node {
  int value;
  struct node* next;
};

struct node* node_allocate(int v) {
  struct node* nnode = malloc(sizeof(struct node));
  nnode->value = v;
  nnode->next = 0;
  return nnode;
}

void sqPop(struct node** headP) {
  if(*headP == 0) {
    return;
  }
  struct node* cleanup = *headP;
  *headP = (*headP)->next;
  free(cleanup);
}

void sqEnqueue(int v, struct node** headP) {
  struct node* nnode = node_allocate(v);
  if(*headP == 0) {
    *headP = nnode;
    return;
  }
  
  for(struct node* ptr = *headP; ptr != 0; ptr = ptr->next) {
    if(ptr->next == 0) {
      ptr->next = nnode;
      return;
    }
  }
}

void sqPush(int v, struct node** headP) {
  struct node* nnode = node_allocate(v);

  nnode->next = *headP;
  *headP = nnode;
}

void gCleaner(struct node* head) {
  struct node* ptr = 0;
  while(head != 0) {
    ptr = head;
    head = head->next;
    free(ptr);
  }
}

int main(int argc, char* argv[argc+1]) {

  FILE* instruct = fopen(argv[1], "r");

  if(instruct == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  //initialize array for reading input, and head node for stack/queue
  char com[8];
  struct node* sqhead = 0;
  
  while(fscanf(instruct, "%s", com) != EOF) {
    if(strcmp(com, "POP") == 0) {
      sqPop(&sqhead);
    }
    else if(strcmp(com, "ENQUEUE") == 0) {
      int number;
      fscanf(instruct, "%d\n", &number);
      sqEnqueue(number, &sqhead);
    }
    else if(strcmp(com, "PUSH") == 0) {
      int number;
      fscanf(instruct, "%d\n", &number);
      sqPush(number, &sqhead);
    }
    
    if(sqhead == 0) {
      printf("EMPTY\n");
    }
    else {
      for(struct node* ptr = sqhead; ptr != 0; ptr = ptr->next) {
	printf("%d ", ptr->value);
      }
      printf("\n");
    }
  }

  gCleaner(sqhead);
  fclose(instruct);
  
  return EXIT_SUCCESS;
}
