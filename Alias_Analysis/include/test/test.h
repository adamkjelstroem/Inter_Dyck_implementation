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


     graph buildSimple(int k){
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

     graph buildNSquared(int l){
          graph g;

          //alpha = (
          //beta = [

          //special vertices
          int u = 0;
          int u2 = u+1;
          int x = u2+1;
          int x2 = x+1;
          int v = x2+1;
          int v2 = v+1;

          //special vertices
          g.getVertex(u, 0, "");
          g.getVertex(u2, 0, "");
          g.getVertex(x, 0, "");
          g.getVertex(x2, 0, "");
          g.getVertex(v, 0, "");
          g.getVertex(v2, 0, "");

          //special edges
          g.addEdge(u, 0, u2, 0, "[");
          g.addEdge(x, 0, x2, 0, "[");
          g.addEdge(x, 0, x2, 0, "(");
          g.addEdge(v2, 0, v, 0, "(");

          //add 'l' edges with ( from v2 to v
          
          helper(&g, v2,  v, l,     v2+1, "(");

          helper(&g,  x,  u, l,   v2+1+l, "[");

          helper(&g,  v,  x, l, v2+1+2*l, "(");

          helper(&g,  v, v2, l, v2+1+3*l, "[");
          
          g.dsu.init(g.N);
          g.initWorklist();

          return g;
     }

     void helper(graph* h, int start,  int end, int count, int space, string label){
          h->getVertex(start, 0, "");
          h->getVertex(end, 0, "");
          h->addEdge(start, 0, space, 0, label);
          for(int i = 0; i < count-2; i++){
               h->getVertex(space + i, 0, "");
               h->getVertex(space + i + 1, 0, "");
               h->addEdge(space+i, 0, space+i+1, 0, label);
          }
          h->getVertex(count-2+space, 0, "");
          h->addEdge(count-2+space, 0, end, 0, label);
     }
};

#endif