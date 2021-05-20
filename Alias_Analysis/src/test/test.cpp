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
        int height = 4;
        graph g = buildSimple(height);
        g = g.flatten("(", height+1);

        g.initWorklist();

        g.bidirectedReach();
        
        if(g.calcNumReachablePairs() != 1){
            g.printGraphAsTikz();
            
            cout<<"graph of type ((([[[)))]]] should always have 1 reachable pair, but "<<g.calcNumReachablePairs()<<" found when flattening to "<<height<<endl;
            
            return false;
        }
    }

    { //testing copy
        int height = 4;
        graph g = Test::makeRandomGraph(10, 10, 10);
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

    { //test that flatten on "[" and "(" up to the expected bound yields the same results
        int height = 4;
        graph g = buildSimple(height);
        graph g2 = g.copy();

        g = g.flatten("(", height+1);
        g2 = g2.flatten("[", height+1);


        g.bidirectedReach();
        g2.bidirectedReach();

        if(g.calcNumReachablePairs() != 1 || g.calcNumReachablePairs() != g2.calcNumReachablePairs()){
            cout<<"Number of reachable pairs should be the same when flattening on either";
            cout<<" parenthesis or bracket, but they're not!"<<endl;

            cout<<"For parenthesis:"<<endl;
            g.printGraphAsTikz();
            cout<<"For brackets: "<<endl;
            g2.printGraphAsTikz();
            return false;
        }

    }

    {
        for(int seed = 0; seed < 10; seed++){

            //compute number of reachable pairs for many graphs using
            //1) flatten on parenthesis, then compute
            //2) flatten on brackets, then compute
            //3) flattenReach on parenthesis
            //4) flattenReach on brackets
            // if these don't all match, print the original graph
            
            graph original, g1, g2, g3, g4;
            
            original = Test::makeRandomGraph(seed, 10, 7);

            

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
