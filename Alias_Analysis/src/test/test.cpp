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

    //generates random graphs and verifies that different flattening techniques produce same result
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

            
            g1 = original.copy();
            g2 = original.copy();
            g3 = original.copy();
            g4 = original.copy();

            g1 = g1.flatten("(", 20);
            g1.bidirectedReach();
            int g1Pairs = g1.calcNumReachablePairs();
            
            g2 = g2.flatten("[", 20);
            g2.bidirectedReach();
            int g2Pairs = g2.calcNumReachablePairs();
            

            /*
            g3.flattenReach("(");
            int g3Pairs = g3.calcNumReachablePairs();
            cout<<"g3 done"<<endl;

            g4.flattenReach("[");
            int g4Pairs = g4.calcNumReachablePairs();
            cout<<"g4 done"<<endl;
            */
            if(!(g1Pairs == g2Pairs)){
                cout<<endl<<endl;

                original.printGraphAsTikz();

                cout<<endl<<endl;

                cout<<"Reachability details for flattenReach on (:"<<endl;
                g3.printDetailReach();

                cout<<"Reachability details for flattenReach on [:"<<endl;
                g4.printDetailReach();

                graph g5 = original.copy();
                g5.flattenReach("(");
                graph g6 = original.copy();
                g6.flattenReach("[");

                cout<<"Found an exception example!"<<endl;
                cout<<"flatten on (: "<<g1Pairs<<endl;
                cout<<"flatten on [: "<<g2Pairs<<endl;
                original.printAsDot();
                

                return false;
            }

        }
    }

    //testing that flattenReach on brackets and flatten then reach on brackets yeilds same result
    {
        int height = 3;
        graph g1 = buildSimple(height);
        graph g2 = g1.copy();

        g1.flattenReach("[");

        
        g2 = g2.flatten("(", height+1);
        g2.bidirectedReach();

        int numg1 = g1.calcNumReachablePairs();
        int numg2 = g2.calcNumReachablePairs();
        if(numg1 != numg2){
            cout<<"number of reachable pairs is different when doing flattenReach ("<<numg1<<") and when doing flatten, then reach ("<<numg2<<") on brackets"<<endl;
            return false;
        }
    }

    //testing that flattenReach on brackets and parentheses should be the same for this simple case
    {
        int height = 3;
        graph g = buildSimple(height);
        graph g2 = g.copy();
        graph g3 = g.copy();

        g.flattenReach("(");
        g2.flattenReach("[");

        int numg = g.calcNumReachablePairs();
        int numg2 = g2.calcNumReachablePairs();

        g3 = g3.flatten("(", height+1);
        g3.bidirectedReach();

        if(numg != numg2){
            cout<<"Number of reachable pairs should be the same when flattening on either";
            cout<<" parenthesis or bracket, but they're not! For parenthesis: "<<numg<<". for brackets: "<<numg2<<endl;
            cout<<"result should be: "<<g3.calcNumReachablePairs()<<endl;
            return false;
        }
    }

    //testing for random graphs that all 4 different techniques produce same results
    {
        int flattenHeight = 20;
        for(int seed = 0; seed < 10; seed++){
            graph orig = makeRandomGraph(seed, 10, 10);

            graph g1 = orig.copy();
            graph g2 = orig.copy();
            graph g3 = orig.copy();
            graph g4 = orig.copy();

            g1.flattenReach("(");
            int num1 = g1.calcNumReachablePairs();

            g2.flattenReach("[");
            int num2 = g2.calcNumReachablePairs();

            g3 = g3.flatten("(", flattenHeight);
            g3.bidirectedReach();
            int num3 = g3.calcNumReachablePairs();

            g4 = g4.flatten("[", flattenHeight);
            g4.bidirectedReach();
            int num4 = g4.calcNumReachablePairs();

            if(!(num1 == num2 && num2 == num3 && num3 == num4)){
                orig.printGraphAsTikz();

                cout<<"Number of reachable pairs should be the same for all 4 methods!"<<endl;
                cout<<"flattenReach on parenthesis: "<<num1<<endl;
                cout<<"flattenReach on bracket: "<<num2<<endl;
                cout<<"flatten, then reach on prenthesis up to height "<<flattenHeight<<": "<<num3<<endl;
                cout<<"flatten, then reach on brackets up to height "<<flattenHeight<<": "<<num3<<endl;
            }
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
