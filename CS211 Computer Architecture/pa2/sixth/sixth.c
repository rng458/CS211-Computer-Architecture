#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

struct node {
  int id;
  int visited;
  int distance;
  char* name;
  struct edge* next;
};

struct edge {
  int id;
  int weight;
  char* name;
  struct edge* next;
};

struct stack {
  struct node* vertex;
  struct stack* next;
};

struct node** makeGraph(int graphSize) {
  struct node** nGraph = malloc(graphSize*sizeof(struct node*));
  return nGraph;
}

struct node* makeVert(char* name, int id) {
  struct node* nVert = malloc(sizeof(struct node));
  nVert->id = id;
  nVert->visited = 0;
  nVert->distance = INT_MAX;
  nVert->name = name;
  nVert->next = 0;
  return nVert;
}

struct edge* makeEdge(char* name, int id, int w) {
  struct edge* nEdge = malloc(sizeof(struct edge));
  nEdge->id = id;
  nEdge->weight = w;
  nEdge->name = malloc(32*sizeof(char));
  strcpy(nEdge->name, name);
  nEdge->next = 0;
  return nEdge;
}

struct stack* makeSVert(struct node* vertex) {
  struct stack* newSVert = malloc(sizeof(struct stack));
  newSVert->vertex = vertex;
  newSVert->next = 0;
  return newSVert;
}

struct node* copyVert(struct node* ogVert) {
  struct node* newVert = malloc(sizeof(struct node));
  newVert->id = ogVert->id;
  newVert->visited = ogVert->visited;
  newVert->distance = ogVert->distance;
  newVert->name = malloc(32*sizeof(char));
  strcpy(newVert->name, ogVert->name);
  newVert->next = 0;
  return newVert;
}

void sortVertices(struct node** graph, int graphSize) {
  for(int i = 0; i < graphSize; i++) {
    for(int j = i+1; j < graphSize; j++) {
      if(strcmp(graph[j]->name, graph[i]->name) < 0) {
	struct node* tPtr = graph[i];
	graph[i] = graph[j];
	graph[j] = tPtr;
      }
    }
  }
}

struct node* findSmallest(struct node** graph, int graphSize) {
  struct node* smallest = 0;
  for(int i = 0; i < graphSize; i++) {
    if(!graph[i]->visited){
      if(smallest == 0) {
	smallest = graph[i];
      }
      else if(strcmp(graph[i]->name, smallest->name) < 0) {
	smallest = graph[i];
      }
    }
  }
  return smallest;
}

int allVisited(struct node** graph, int graphSize) {
  for(int i = 0; i < graphSize; i++) {
    if(graph[i]->visited == 0) {
      return 0;
    }
  }
  return 1;
}

void freeGraph(struct node** graph, int graphSize) {
  for(int i = 0; i < graphSize; i++) {
    struct node* head = graph[i];
    struct edge* prev = 0;
    struct edge* ptr = head->next;
    while(ptr != 0) {
      prev = ptr;
      ptr = ptr->next;
      free(prev->name);
      free(prev);
    }
    free(head->name);
    free(head);
  }
  free(graph);
}

//in case i need this lmao
void resetVisited(struct node** graph, int graphSize) {
  for(int i = 0; i < graphSize; i++) {
    graph[i]->visited = 0;
  }
}

void sPush(struct stack** stackHead, struct node* insVert) {
  struct stack* newStackNode = makeSVert(insVert);

  newStackNode->next = *stackHead;
  *stackHead = newStackNode;
}

struct stack* sPop(struct stack** stackHead) {
  if(*stackHead == 0) {
    return 0;
  }

  struct stack* target = *stackHead;
  *stackHead = (*stackHead)->next;

  return target;
}

int inStack(struct stack* stackHead, struct node* vertex) {
  for(struct stack* sPtr= stackHead; sPtr != 0; sPtr = sPtr->next) {
    if(strcmp(sPtr->vertex->name, vertex->name) == 0) {
      return 1;
    }
  }
  return 0;
}

void DFS(struct node** graph, struct node* vertex, struct stack** stackHead) {
  graph[vertex->id]->visited = 1;

  for(struct edge* eptr = vertex->next; eptr != 0; eptr = eptr->next) {
    if(!graph[eptr->id]->visited) {
      DFS(graph, graph[eptr->id], stackHead);
    }
  }

  if((*stackHead)->vertex == 0) {
    (*stackHead)->vertex = vertex;
  }
  else {
    sPush(stackHead, vertex);
  }   
}

struct node** TPS(struct node** graph, int graphSize, struct node* initVert) {

  resetVisited(graph, graphSize);

  //int cycle = 0;
  struct stack** topoStack = malloc(sizeof(struct stack*)); //this is the source of problems
  *topoStack = malloc(sizeof(struct stack));
  (*topoStack)->vertex = 0;
  (*topoStack)->next = 0;

  for(int i = 0; i < graphSize; i++) {
    if(!graph[i]->visited) {
      DFS(graph, graph[i], topoStack);
    }
  }

  struct node** tsGraph = makeGraph(graphSize);
  
  for(int i = 0; i < graphSize; i++) {
    struct stack* currentStack = sPop(topoStack);
    struct node* temp = copyVert(currentStack->vertex);
    tsGraph[i] = temp;
    free(currentStack);
  }
  
  free(topoStack);
  return tsGraph;
}

void DAGSSP(struct node** graph, int graphSize, struct node* sourceVert) {
  
  struct node** tSortedGraph = TPS(graph, graphSize, sourceVert);

  for(int i = 0; i < graphSize; i++) {
    graph[i]->distance = INT_MAX;
  }
  
  graph[sourceVert->id]->distance = 0;

  for(int i = 0; i < graphSize; i++) {
    struct node* vertU = tSortedGraph[i];
    for(struct edge* ePtr = graph[vertU->id]->next; ePtr != 0; ePtr = ePtr->next) {
      if(graph[vertU->id]->distance != INT_MAX) {
	if((graph[ePtr->id]->distance) > ((graph[vertU->id]->distance) + ePtr->weight)) {
	  graph[ePtr->id]->distance = ((graph[vertU->id]->distance) + ePtr->weight);
	}
      }
    }
  }

  for(int i = 0; i < graphSize; i++) {
    if(graph[i]->distance != INT_MAX) {
      printf("%s %d\n", graph[i]->name, graph[i]->distance);
    }
    else if(graph[i]->distance == INT_MAX) {
      printf("%s INF\n", graph[i]->name);
    }
  }

  for(int i = 0; i < graphSize; i++) {
    free(tSortedGraph[i]->name);
    free(tSortedGraph[i]);
  }
  free(tSortedGraph);
  
}

int main (int argc, char* argv[argc+1]) {

  FILE* file1 = fopen(argv[1], "r");
  FILE* file2 = fopen(argv[2], "r");

  if(file1 == 0 || file2 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int graphSize;
  fscanf(file1, "%d\n", &graphSize);
  struct node** daGraph = makeGraph(graphSize);
  for(int i = 0; i < graphSize; i++) {
    char* vertName = malloc(32*sizeof(char));
    fscanf(file1, "%s\n", vertName);
    struct node* nVert = makeVert(vertName, -1);
    daGraph[i] = nVert;
  }
  
  sortVertices(daGraph, graphSize);
  for(int i = 0; i < graphSize; i++) {
    daGraph[i]->id = i;
  }

  char vert1name[32];
  char vert2name[32];
  int weight = 0;
  while(fscanf(file1, "%s %s %d\n", vert1name, vert2name, &weight) != EOF) {
    struct node* vert1ptr = 0;
    struct node* vert2ptr = 0;

    for(int i = 0; i < graphSize; i++) {
      if(strcmp(daGraph[i]->name, vert1name) == 0) {
	vert1ptr = daGraph[i];
      }
      if(strcmp(daGraph[i]->name, vert2name) == 0) {
	vert2ptr = daGraph[i];
      }
      if((vert1ptr != 0) && (vert2ptr != 0)) {
	break;
      }
    }

    struct edge* wEdge = makeEdge(vert2ptr->name, vert2ptr->id,  weight);

    if(vert1ptr->next == 0) {
      vert1ptr->next = wEdge;
    }
    else if(strcmp(wEdge->name, vert1ptr->next->name) < 0) {
      wEdge->next = vert1ptr->next;
      vert1ptr->next = wEdge;
    }
    else {
      struct edge* eptr = vert1ptr->next;
      while(eptr->next != 0) {
	if(strcmp(wEdge->name, eptr->next->name) < 0) {
	  break;
	}
	eptr = eptr->next;
      }
      wEdge->next = eptr->next;
      eptr->next = wEdge;
    } 
  }

  char query[32];
  while(fscanf(file2, "%s\n", query) != EOF) {
    int qID = 0;
    for(int i = 0; i < graphSize; i++) {
      if(strcmp(query, daGraph[i]->name) == 0) {
	qID = daGraph[i]->id;
      }
    }
    DAGSSP(daGraph, graphSize, daGraph[qID]);
    printf("\n");
  }
  
  freeGraph(daGraph, graphSize);
  fclose(file1);
  fclose(file2);
  return EXIT_SUCCESS;
}
