//Sarah Kazi
//CS 401
//Shortest Constrained Path
//4/12/17
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef int Vertex;

typedef struct Edge
{
    Vertex  src;
    Vertex  dest;
    int     cost;
    int     time;
    struct  Edge *next;
} Edge;

typedef struct Graph
{
    Edge      **Vertices;
    int       NumVertices;
    int       NumEdges;
    int 		cap; //capacity
    
} Graph;
typedef struct MinHeapTuple
{
	int v;
	int distance;

}MinHeapTuple;
typedef struct MinHeap
{
	int size;
	int cap;
	struct MinHeapTuple **arr;
}MinHeap;

MinHeap* initMinHeap(int capacity){
	MinHeap* mh = (MinHeap *)malloc(sizeof(MinHeap));
	mh->size = 0;
	mh->cap = capacity;
	mh->arr = (MinHeapTuple**)malloc(capacity*sizeof(MinHeapTuple*));
	return mh;
	
}
MinHeapTuple* initNode(int v, int distance){
	MinHeapTuple *mht = (MinHeapTuple*)malloc(sizeof(MinHeapTuple));
	mht->v = v; 
	mht->distance = distance;
	return mht;
}
int isEmptyMinHeap(MinHeap* mh){
	return mh->size == 0;
}
int AddEdge(Graph *G, int src, int dest, int c, int t){
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    edge->src = src;
    edge->dest = dest;
    edge->cost = c;
    edge->time = t;
    
    Edge *prev = NULL;
    Edge *cur = G->Vertices[src];
    while(cur != NULL){
        if (dest <= cur->dest)  // insert here!
            break;
        
        // else keep going:
        prev = cur;
        cur = cur->next;
    }
    if (prev == NULL)  // update head pointer:
    {
        edge->next = G->Vertices[src];
        G->Vertices[src] = edge;
    }
    else  // insert between prev and cur:
    {
        edge->next = prev->next;
        prev->next = edge;
    }
    
    //
    // done:
    //
    G->NumEdges++;
    return 1;
    
    
}

Vertex *Neighbors(Graph *G, Vertex v)
{
    Vertex *neighbors;
    int     N;
    int     i;
    
    if (v < 0 || v >= G->NumVertices)  // invalid vertex #:
        return NULL;
    
    //
    // allocate array of worst-case size: # of vertices + 1
    //
    N = G->NumVertices + 1;
    
    neighbors = (Vertex *)malloc(N * sizeof(Vertex));
    if (neighbors == NULL)
    {
        printf("\n**Error in Neighbors: malloc failed to allocate\n\n");
        exit(-1);
    }
    
    //
    // Now loop through the list of edges and copy the dest
    // vertex of each edge:
    //
    Edge *cur = G->Vertices[v];
    
    i = 0;
    while (cur != NULL)  // for each edge out of v:
    {
        //
        // the dest is our neighbor --- however, we have to be
        // careful of multi-edges, i.e. edges with the same dest.
        // Since edges are stored in order, edges with same dest
        // appear next to each other in the list --- so look to
        // see if array already contains dest before we copy over:
        //
        if (i == 0)  // first neighbor, we always copy:
        {
            neighbors[i] = cur->dest;
            ++i;
        }
        else if (neighbors[i - 1] != cur->dest)  // make sure not a multi-edge:
        {
            neighbors[i] = cur->dest;
            ++i;
        }
        else // multi-edge, so ignore:
            ;
        
        cur = cur->next;
    }
    
    //
    // follow last element with -1 and return:
    //
    neighbors[i] = -1;
    
    return neighbors;
}

void PrintGraph(Graph *G, char *title, int complete)
{
    printf(">>Graph: %s\n", title);
    printf("  # of vertices: %d\n", G->NumVertices);
    printf("  # of edges:    %d\n", G->NumEdges);
    
    // is a complete print desired?  if not, return now:
    if (!complete)
        return;
    
    //
    // Otherwise dump complete graph info:
    //
    printf("  Adjacency Lists:\n");
    
    int  v;
    for (v = 0; v < G->NumVertices; ++v)
    {
   
        
        Edge *edge = G->Vertices[v];
        while (edge != NULL)
        {
            printf("(%d,%d,%d, %d)", edge->src, edge->dest, edge->cost, edge->time);
            
            edge = edge->next;
            if (edge != NULL)
                printf("\n");
        }
        
        printf("\n");
    }
    
    //
    // neighbors:
    //
    printf("  Neighbors:\n");
    
    for (v = 0; v < G->NumVertices; ++v)
    {

        
        Vertex *neighbors = Neighbors(G, v);
        
        if (neighbors == NULL)
            printf("**ERROR: Neighbors returned NULL.\n\n");
        else
        {
            int  j;
            
            for (j = 0; neighbors[j] != -1; ++j)
            {
                printf("%d, ", neighbors[j]);
            }
            
            printf("-1\n");
            
 
        }
    }
   
}

Graph *CreateGraph(char filename[]){
    Graph *G;
    FILE  *pFile;
    int   i;
    int   N;

    int src, dest, cost, time;
    pFile = fopen(filename, "r");
    if(pFile == NULL){
        printf("Couldn't locate file %s. Exiting...\n", filename);
        exit(-1);
    }
    //make sure vertices >=1
    fscanf(pFile, "%d", &N); //read in # of vertices
    
    if(N < 1){
        printf("Not enough vertices...\n");
        return NULL;
    }
    else
        printf("Read in %d vertices\n", N);
    
    
    //allocate space for the graph
    G = (Graph *)malloc(sizeof(Graph));
    G->NumVertices = N;
    G->Vertices = (Edge **)malloc(N * sizeof(Edge *));
    
    //initialize vertices to null
    for(i= 0; i < N; i++)
        G->Vertices[i] = NULL;
    
    G->NumVertices = 0;
    G->NumEdges    = 0;
    G->cap         = N;
    
    while(!feof(pFile)) {
        fscanf(pFile, "%d %d %d %d", &src, &dest, &cost, &time);
        AddEdge(G, src, dest, cost, time);
        
    }
    printf("%d vertices in graph\n", G->NumVertices);
    fclose(pFile);
    return G;
}

int main(int argc, char *argv[]) {
    Graph *graph;
    
    int src, dest, budget;
   if(argc != 5){
       printf("Didn't enter 5 arguments. Exiting...\n");
       exit(-1);
   }
   else{
        graph = CreateGraph(argv[0]);
        
        PrintGraph(graph, "cpath Graph", 1);
        int src = atoi(argv[1]);
        int dest = atoi(argv[2]);
        int budget = atoi(argv[3]);
    }
    
    
    
    return 0;
    
}