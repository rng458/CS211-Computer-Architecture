#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  int visited;
  char* name;
  struct node* next;
};

struct node* allocateVert(char* identity) {
  struct node* temp = malloc(sizeof(struct node));
  temp->visited = 0;
  temp->name = identity;
  temp->next = 0;
  return temp;
}

struct node* copyVert(struct node* ogVert) {
  struct node* temp = malloc(sizeof(struct node));
  temp->visited = ogVert->visited;
  temp->name = malloc(32*sizeof(char));
  strcpy(temp->name, ogVert->name);
  temp->next = 0;
  return temp;
}

int isInQueue(struct node* qHead, struct node* target) {
  struct node* ptr = qHead;

  while(ptr != 0) {
    if(strcmp(target->name, ptr->name) == 0) {
      return 1;
    }
    ptr = ptr->next;
  }
  
  return 0;
}

void enqueue(struct node* qHead, struct node* INS) {
  struct node* ptr = qHead;
  while(ptr->next != 0) {
    ptr = ptr->next;
  }
  ptr->next = INS;
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

  char id[32];
  while(fscanf(file2, "%s\n", id) != EOF) {
    struct node* queue = 0;
    for(int i = 0; i < graphSize; i++) {
      if(strcmp(mGraph[i]->name, id) == 0) {
	queue = copyVert(mGraph[i]); //to initialize the queue
	break;
      }
    }

    struct node* qPtr = queue;
    while(qPtr != 0) {
      struct node* ptr = 0;
      for(int i = 0; i < graphSize; i++) {
	if((strcmp(qPtr->name, mGraph[i]->name) == 0) && (!mGraph[i]->visited)) {
	  mGraph[i]->visited = 1;
	  ptr = mGraph[i];
	  break;
	}
      }
      
      for(ptr = ptr; ptr != 0; ptr = ptr->next) {
	if((ptr->visited) || (isInQueue(queue, ptr))) {
	  continue;
	}
	else {
	  struct node* temp = copyVert(ptr);
	  enqueue(queue, temp);
	}
      }
      qPtr = qPtr->next;
    }

    for(struct node* temp = queue; temp != 0; temp = temp->next) {
      printf("%s ", temp->name);
    }
    printf("\n");

    struct node* cleaner = queue;
    while(queue != 0) {
      queue = queue->next;
      free(cleaner->name);
      free(cleaner);
      cleaner = queue;
    }
    
    for(int i = 0; i < graphSize; i++) {
      mGraph[i]->visited = 0;
    }    
  }

  freeGraph(mGraph, graphSize);
  fclose(file1);
  fclose(file2);
  return EXIT_SUCCESS;
}
