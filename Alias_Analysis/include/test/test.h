#ifndef TEST_H
#define TEST_H


#include "graph/graph.h"

using namespace std;

class Test{
public:
     bool test();
     
     static graph makeRandomGraph(int seed, int edges, int vertices){
          graph g;

          srand(seed);

          for(int i = 0; i < edges; i++){
               int a = rand() % vertices;
               int b = rand() % (vertices-1);
               if (b >= a) b++; //guarantees a != b
               string field = "[";
               if(rand() % 2 == 0) field = "(";
               g.addEdge((a), 0, (b), 0, field);
          }
          g.dsu.init(g.vertices.size());

          return g;
     }


     static graph buildSimple(int k){
          graph g;
          
          for(int i = 0; i < k; i++){
               g.getVertex(i, 0, "");
               g.addEdge(i, 0, i+1, 0, "(");
          }
          for(int i = 0; i < k; i++){
               g.getVertex((i+k), 0, "");
               g.addEdge((i+k), 0, (i+k+1), 0, "[");
          }
          for(int i = 0; i < k; i++){
               g.getVertex((i+k*2), 0, "");
               g.addEdge((i+k*2+1), 0, (i+k*2), 0, "(");
          }
          for(int i = 0; i < k; i++){
               g.getVertex((i+k*3), 0, "");
               g.addEdge((i+k*3+1), 0, (i+k*3), 0, "[");
          }

          g.dsu.init(g.vertices.size());
          g.initWorklist();
               
          
          return g;  
     }
};

#endif