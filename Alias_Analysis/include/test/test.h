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
               g.addEdge(to_string(a), 0, to_string(b), 0, field);
          }
          g.dsu.init(g.vertices.size());

          return g;
     }
};

#endif