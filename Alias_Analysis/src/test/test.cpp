#include "test/test.h"
#include "graph/graph.h"

#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>

graph buildSimple(int k);

bool Test::test(){
    
    {
        //testing () graph works (single dyck)
        graph g;
        for(int i = 0; i < 7; i++){
            g.getVertex(to_string(i), 0);
        }
        g.addEdge("0", 0, "1", 0, "(");
        g.addEdge("2", 0, "1", 0, "(");
        g.dsu.init(g.vertices.size());
        g.initWorklist();

        g.bidirectedReach();

        if(g.calcNumReachablePairs() != 1){
            cout<<"graph of type () should have 1 reachable pair"<<endl;
            return false;
        }

    }

    //testing (([[))]] type graphs work
    {
        int height = 2;
        graph g = buildSimple(height);
        g = g.flatten("(", height+1);

        g.initWorklist();

        g.bidirectedReach();
        
        g.printDetailReach();

        if(g.calcNumReachablePairs() != 1){
            cout<<"graph of type ((([[[)))]]] should always have 1 reachable pair, but "<<g.calcNumReachablePairs()<<" found when flattening to "<<height<<endl;

            g.printGraphAsTikz();

            return false;
        }
    }

    { //testing copy
        int height = 4;
        graph g = g.makeRandomGraph(10, 10, 10);
        graph g2 = g.copy();


        g = g.flatten("(", height+1);
        g2 = g2.flatten("(", height+1);


        g.bidirectedReach();
        g2.bidirectedReach();

        if(g.calcNumReachablePairs() != g2.calcNumReachablePairs()){
            cout<<"copying graph should result in same number of reachable pairs for either"<<endl;
            return false;
        }

    }

    { //test that flatten on "[" and "(" up to <above> the expected bound yields the same results
        int height = 4;
        graph g = buildSimple(height);
        graph g2 = g.copy();

        g = g.flatten("(", height+1);
        g2 = g2.flatten("[", height+1);


        g.bidirectedReach();
        g2.bidirectedReach();

        if(g.calcNumReachablePairs() != g2.calcNumReachablePairs()){
            cout<<"error message TODO"<<endl;
            return false;
        }

    }


    return true;
}



graph buildSimple(int k){
    graph g;
    
    for(int i = 0; i < k; i++){
        g.getVertex(to_string(i), 0);
        g.addEdge(to_string(i), 0, to_string(i+1), 0, "(");
    }
    for(int i = 0; i < k; i++){
        g.getVertex(to_string(i+k), 0);
        g.addEdge(to_string(i+k), 0, to_string(i+k+1), 0, "[");
    }
    for(int i = 0; i < k; i++){
        g.getVertex(to_string(i+k*2), 0);
        g.addEdge(to_string(i+k*2+1), 0, to_string(i+k*2), 0, "(");
    }
    for(int i = 0; i < k; i++){
        g.getVertex(to_string(i+k*3), 0);
        g.addEdge(to_string(i+k*3+1), 0, to_string(i+k*3), 0, "[");
    }

    g.dsu.init(g.vertices.size());
    g.initWorklist();
        
    
    return g;  
}