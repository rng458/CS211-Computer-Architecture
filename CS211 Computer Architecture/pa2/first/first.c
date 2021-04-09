#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  char* name;
  struct node* next;
};

struct node* allocateVert(char* identity) {
  struct node* temp = malloc(sizeof(struct node));
  temp->name = identity;
  temp->next = 0;
  return temp;
}

struct node** makeGraph(int gSize) {
  struct node** nGraph = malloc(gSize*sizeof(struct node*));
  return nGraph;
}

void freeGraph(struct node** graph, int gSize) {
  for(int i = 0; i < gSize; i++) {
    struct node* ptr = graph[i];
    while(ptr != 0) {
      struct node* temp = ptr;
      ptr = ptr->next;
      free(temp->name);
      free(temp);
    }
  }
  free(graph);
}

/*void printGraph(struct node** graph, int gSize) {
  for(int i = 0; i < gSize; i++) {
    for(struct node* ptr = graph[i]->next; ptr != 0; ptr = ptr->next) {
      printf("In %s: node %s\n", graph[i]->name, ptr->name);
    }
  }
  }*/

int main(int argc, char* argv[argc+1]) {

  FILE* file1 = fopen(argv[1], "r");
  FILE* file2 = fopen(argv[2], "r");

  if(file1 == 0 || file2 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int graphSize;
  fscanf(file1, "%d\n", &graphSize);
  struct node** mGraph;
  mGraph = makeGraph(graphSize);
  
  for(int i = 0; i < graphSize; i++) {
    char* vertnameptr = malloc(32*sizeof(char));
    fscanf(file1, "%s\n", vertnameptr);
    struct node* nVert = allocateVert(vertnameptr);
    mGraph[i] = nVert;
  }

  char vert1[32];
  char vert2[32];
  while(fscanf(file1, "%s %s\n", vert1, vert2) != EOF) {
    struct node* head1 = 0;
    struct node* head2 = 0;

    for(int i = 0; i < graphSize; i++) {
      if(strcmp(vert1, mGraph[i]->name) == 0) {
	head1 = mGraph[i];
      }
      if(strcmp(vert2, mGraph[i]->name) == 0) {
	head2 = mGraph[i];
       }
      if(head1 != 0 && head2 != 0) {
	break;
      }
     }

    char* vert2name = malloc(32*sizeof(char));
    strcpy(vert2name, vert2);
    char* vert1name = malloc(32*sizeof(char));
    strcpy(vert1name, vert1);
    
    struct node* dynVert = allocateVert(vert2name);
    while(head1->next != 0) {
      if(strcmp(dynVert->name, head1->next->name) < 0) {
	break;
      }
      head1 = head1->next;
    }
    dynVert->next = head1->next;
    head1->next = dynVert;

    dynVert = allocateVert(vert1name);
    while(head2->next != 0) {
      if(strcmp(dynVert->name, head2->next->name) < 0) {
	break;
      }
      head2 = head2->next;
    }
    dynVert->next = head2->next;
    head2->next = dynVert;
  }

  char operation;
  char id[32];
  while(fscanf(file2,"%c %s\n", &operation, id) != EOF) {
    struct node* ptr = 0;
    for(int i = 0; i < graphSize; i++) {
      if(strcmp(id, mGraph[i]->name) == 0) {
	ptr = mGraph[i]->next;
	break;
      }
    }
    
    if(operation == 'd') {
      int degree = 0;
      while(ptr != 0) {
	ptr = ptr->next;
	degree++;
      }
      printf("%d\n", degree);
    }
    else if(operation == 'a') {
      while(ptr != 0) {
	printf("%s ", ptr->name);
	ptr = ptr->next;
      }
      printf("\n");
    }
  }
  
  //printGraph(mGraph, graphSize);
  freeGraph(mGraph, graphSize);

  fclose(file1);
  fclose(file2);
  return EXIT_SUCCESS;
}
