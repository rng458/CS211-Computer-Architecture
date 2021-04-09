#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node {
  int id;
  int visited;
  char* name;
  struct edge* next;
};

struct edge {
  int id;
  int weight;
  char* name;
  struct edge* next;
};

struct node** makeGraph(int graphSize) {
  struct node** nGraph = malloc(graphSize*sizeof(struct node*));
  return nGraph;
}

struct node* makeVert(char* name, int id) {
  struct node* nVert = malloc(sizeof(struct node));
  nVert->id = id;
  nVert->visited = 0;
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

void DFS(struct node** graph, struct node* vertex) {
  
  graph[vertex->id]->visited = 1;
  printf("%s ", graph[vertex->id]->name);
  struct edge* eptr = vertex->next;

  while(eptr != 0) {
    if(graph[eptr->id]->visited) {
      eptr = eptr->next;
      continue;
    }
    else {
      DFS(graph, graph[eptr->id]);
      eptr = eptr->next;
    }
  }

}

int main (int argc, char* argv[argc+1]) {

  FILE* file1 = fopen(argv[1], "r");

  if(file1 == 0) {
    printf("error\n");
    return EXIT_SUCCESS;
  }

  int graphSize;
  fscanf(file1, "%d\n", &graphSize);
  struct node** daGraph = makeGraph(graphSize);
  for(int i = 0; i < graphSize; i++) {
    char* vertName = malloc(32*sizeof(char));
    fscanf(file1, "%s\n", vertName);
    struct node* nVert = makeVert(vertName, i);
    daGraph[i] = nVert;
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

  while(!allVisited(daGraph, graphSize)) {
    DFS(daGraph, findSmallest(daGraph, graphSize));
  }
  
  printf("\n");
  
  freeGraph(daGraph, graphSize);
  fclose(file1);
  return EXIT_SUCCESS;
}
