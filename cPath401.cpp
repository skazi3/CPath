//Sarah Kazi
//CS 401
//Shortest Constrained Path
//4/12/17

#include <iostream>
#include <cstdlib>
#include <iterator>
#include <exception>
#include <stdexcept>
#include <string>
#include <cmath>
#include <cassert>
#include <vector>
#include <functional>
#include <queue>
#include <limits>

using namespace std;
#define INF INT_MAX
typedef int Vertex;

typedef pair<pair<int, int>, int> Triple;
class Compare
{
public:
    bool operator()(Triple p1, Triple p2)
    {
        if(p1.first.first < p2.first.first)
            return true;
        else if(p1.first.first == p2.first.first){
            if (p1.first.second > p2.first.second)
                return true;
            else
                return false;
        }
        else
            return false;
            
    }
};
bool deleteNon( pair<int, int> p1,  pair<int, int> p2){
    if(p1.first == p2.first && p1.second > p2.second)
        return true;
    else if(p1.first > p2.first && p1.second == p2.second)
        return true;
    else
        return false;
}

typedef struct List{
    vector<pair<int, int> > S;
    int Label;
    int Cost;
    int Time;
 
}List;
typedef struct Edge
{
    Vertex  src;
    Vertex  dest;
    pair<int, int> weight; // cost, then time
    struct  Edge *next;
} Edge;

typedef struct Graph
{
    Edge      **Vertices;
    int       NumVertices;
    int       NumEdges;
    int       cap; //capacity
    
} Graph;


int AddEdge(Graph *G, int src, int dest, pair<int, int> w){
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    edge->src = src;
    edge->dest = dest;
    edge->weight = w;
    
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
pair<int, int> getEdgeWeight(Graph *G, Vertex src, Vertex dest)
{
    if (src < 0 || src >= G->NumVertices)  // invalid vertex #:
    {
        printf("\n**Error in getEdgeWeight: src vertex (%d) invalid.\n\n", src);
        exit(-1);
    }
    if (dest < 0 || dest >= G->NumVertices)  // invalid vertex #:
    {
        printf("\n**Error in getEdgeWeight: dest vertex (%d) invalid.\n\n", dest);
        exit(-1);
    }
    
    //
    // search src's edge list, note that multi-edges appear together:
    //
    Edge *cur = G->Vertices[src];
    pair<int, int> weight;
    bool hasEdge = false; /*false*/
    
    while (cur != NULL)
    {
        if (dest == cur->dest)  // candidate edge:
        {
            if (!hasEdge) // first edge:
            {
                hasEdge = true;  /*true*/
                weight = cur->weight;
            }
            else if (cur->weight < weight)  // multi-edge:
            {
                weight = cur->weight;  // smaller, so update:
            }
        }
        else if (dest < cur->dest)  // out of order, end search:
            break;
        
        // next edge:
        cur = cur->next;
    }
    
    //
    // did we find an edge?  make sure...
    //
    if (!hasEdge)
    {
        printf("\n**Error in getEdgeWeight: no edge found from %d to %d.\n\n", src, dest);
        exit(-1);
    }
    
    //
    // success:
    //
    return weight;
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

void PrintGraph(Graph *G, string title, int complete)
{
    cout << title << endl;
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
            cout << "(" << edge->src << "," << edge->dest << "," << edge->weight.first << "," << edge->weight.second <<")";
            
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
void findAllPaths(Graph *G, vector<int> &p, priority_queue<Triple, vector<Triple>, Compare> &mh,
                  vector<List> &distance, int src, int dest){
    mh.push(make_pair(make_pair(0, 0), src));
    distance[src].Label = 0;
    distance[src].Cost  = 0;
    distance[src].Time  = 0;
    distance[src].S.push_back(make_pair(0, 0));
    
    while (!mh.empty()) {
        int curV = mh.top().second;
  
        mh.pop();
        
//        if (distance[curV].Label == INF)
//            break;
        Vertex *n = Neighbors(G, curV);
        int j = 0;
        while (n[j] != -1) {
            int adjV = n[j];
            auto edgeWeight = getEdgeWeight(G, curV, adjV);
            int altCost = distance[curV].Cost + edgeWeight.first;
            int altTime = distance[curV].Time + edgeWeight.second;
            if(distance[adjV].S.empty())
                distance[adjV].S.push_back(make_pair(altCost, altTime));
            else{
             
                auto it = distance[adjV].S.back();
  
                if(it.first > altCost && it.second == altTime){
                    distance[adjV].S.pop_back();
                    distance[adjV].S.push_back(make_pair(altCost, altTime));
                }
                else if(it.first == altCost && it.second > altTime){
                    distance[adjV].S.pop_back();
                    distance[adjV].S.push_back(make_pair(altCost, altTime));
                }
                else if(it.first > altCost && it.second < altTime)
                   distance[adjV].S.push_back(make_pair(altCost, altTime));
            }
            //distance[adjV].S.push_back(make_pair(altCost, altTime));
            if(altCost < distance[adjV].Cost){
                distance[adjV].Cost = altCost;
                distance[adjV].Time = altTime;
                mh.push(make_pair(make_pair(altCost, altTime), adjV));
                p[adjV] = curV;
            }
            else if (altCost == distance[adjV].Cost)
                if(altTime < distance[adjV].Time){
                    distance[adjV].Cost = altCost;
                    distance[adjV].Time = altTime;
                    mh.push(make_pair(make_pair(altCost, altTime), adjV));
                    p[adjV] = curV;
                }
            j++;
        }
        free(n);
        
    }
    
}

void sortDistance(vector<List> &d, Graph *G){
    
    
    for(auto i = 0; i < G->NumVertices; i++){
      
        std::sort(d[i].S.begin(), d[i].S.end(), [=](pair<int, int>p1, pair<int, int> p2) {
            if(p1.first > p2.first)
                return true;
            else if(p1.first < p2.first)
                return false;
            else{
                if(p1.second < p2.second){
                  
                    return true;
                }
                else
                    return false;
            }
                
        });
 
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
        exit(-1);
    }
    else
        printf("Read in %d vertices\n", N);
    
    
    //allocate space for the graph
    G = (Graph *)malloc(sizeof(Graph));
 
    G->Vertices = (Edge **)malloc(N * sizeof(Edge *));
    
    //initialize vertices to null
    for(i= 0; i < N; i++)
        G->Vertices[i] = NULL;
    
    G->NumVertices = N;
    G->NumEdges    = 0;
    G->cap         = N;
    
    while(!feof(pFile)) {
        fscanf(pFile, "%d %d %d %d", &src, &dest, &cost, &time);
        auto w = make_pair(cost, time);
        AddEdge(G, src, dest, w);
        
    }
    printf("%d vertices in graph\n", G->NumVertices);
    fclose(pFile);
    return G;
}
void printPath(vector<int> &predecessor, Vertex dest, Vertex src){
    auto cur = dest;
    vector<int> arr;
    while (predecessor[cur] != -1 ) {
        arr.push_back(cur);
        cur = predecessor[cur];
    }
    arr.push_back(src);
    std::reverse(arr.begin(), arr.end());
    for(auto &a: arr){
        cout << a;
    if(a != arr.back())
        cout << "->";
    }
    cout << endl;
}

int main(int argc, char *argv[]) {
    Graph *graph;
    
    priority_queue<Triple, vector<Triple>, Compare> minheap;
    int seeAllPaths = 0;
    
    int src, dest, budget;
    
   if(argc != 5){
       printf("Didn't enter 5 arguments. Exiting...\n");
       exit(-1);
   }
   else{
    
        graph = CreateGraph(argv[1]);
       
        src = atoi(argv[2]);
        dest = atoi(argv[3]);
        budget = atoi(argv[4]);
        
        printf("src:    %d\n", src);
        printf("dest:   %d\n", dest);
        printf("budget: %d\n\n", budget);
        vector<List> dist(graph->NumVertices);

        for(int i = 0; i <graph->NumVertices; i++){
            dist[i].Cost     = INF;
            dist[i].Time     = INF;
            dist[i].Label    = INF;

          
        }
            
        vector<int> predecessor(graph->NumVertices, -1);
        
        char title[] = "CPATH Graph";
        
        PrintGraph(graph, title, 0); /*change to 1 if user wants to print out neighbors and adjacency lists*/
        cout << endl;
        findAllPaths(graph, predecessor, minheap, dist, src, dest);
        sortDistance(dist, graph);
     
        cout << "(1): All Non-dominated paths from " << "'"<< src << "' "<< "to" <<" '"<< dest << "' "<< "are: "<< endl;

        for (const auto &s: dist[dest].S)
            cout << "(" << s.first << "," << s.second <<"), ";
        cout << endl;
        cout << endl;
        
        cout << "(2): According to a budget of '$" << budget << "', the path is:" << endl;
        auto min = INT_MAX;
        pair<int, int> fastest;
        for (const auto &s: dist[dest].S){
            if(s.first <= budget){
                if(s.second < min){
                    min = s.second;
                    fastest = s;
                }
            }
        }
        if(min == INT_MAX)
            cout << "NOT FEASIBLE!" <<endl;
        else{
            cout << "FEASIBLE with the path being:" << endl;
            printPath(predecessor, dest, src);
            cout << "Fastest path costs: '$"<< fastest.first << "' with a time of: "<< fastest.second << endl;
        }
        cout << "TO SEE ALL TRADEOFF CURVES OF PATHS FROM SRC TO EVERY OTHER VERTEX PRESS 1, OR 0 TO EXIT: "<<endl;
        cin >> seeAllPaths;
        if(seeAllPaths){
            for(auto i = 0; i < graph->NumVertices; i++){
                cout << i <<": ";
                for(auto &d: dist[i].S){
                    cout << "(" << d.first << "," << d.second <<"), ";
                }
                cout << endl;
            }
        }
        else
            exit(-1);
    }
    return 0;
    
}
