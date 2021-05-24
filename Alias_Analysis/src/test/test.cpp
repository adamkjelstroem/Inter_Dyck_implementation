#include "test/test.h"
#include "graph/graph.h"

#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>

graph buildSimple(int k);

bool iterateOverEdgesGivesCorrectOrder(){
    graph g;
    g.addEdge(1, 0, 2, 1, "label");

    bool k = false;
    void* data[] = {&k};
    g.iterateOverEdges([](Vertex start, Vertex end, field f, void* extra[]){
        if(start.x == 2 && end.x == 1){
            (*(bool*)extra[0]) = true;
        }
    }, data);
    if(k){
        cout<<"iterating wrongly over edges"<<endl;
        return false;
    }
    return true;
}

bool oneOpenOneCloseParenthesisHas1pair(){
    graph g;
    for(int i = 0; i < 7; i++){
        g.getVertex((i), 0, "");
    }
    g.addEdge(0, 0, 1, 0, "(");
    g.addEdge(2, 0, 1, 0, "(");
    g.dsu.init(g.vertices.size());
    g.initWorklist();

    g.bidirectedReach();

    if(g.calcNumReachablePairs() != 1){
        cout<<"graph of type () should have 1 reachable pair"<<endl;
        return false;
    }
    return true;
}

bool smallInterDyckWorksForFlattenThenReach(){
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
    return true;
}

bool copyMakesIdenticalGraph(){
    //testing copy

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
    return true;
}

bool simpleInterDyckOnFlattenThenReachForBothIsSame(){
    //test that flatten on "[" and "(" up to the expected bound yields the same results
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
    return true;
}

bool noEdgesMeansNoPairs(){
    //test that an edge-less graph has no reachable pairs
    graph g;
    for(int i = 0; i <10; i++){
        g.getVertex((i), 0, "");
    }
    
    g.dsu.init(g.vertices.size());
    g.initWorklist();

    g.bidirectedReach();

    if(g.calcNumReachablePairs() != 0){
        cout<<"Edge-less graph should have no reachable pairs"<<endl;
        return false;
    }
    return true;
}

bool fullyConnectedEpsEdgeGraphHasMaxPairs(){
    //test that fully connected graph has 'many' reachable pairs
    graph g;
    for(int i = 0; i < 10; i++){
        g.addEdge((i), 0, (i+1), 0, "eps");
    }
    //scc should be of size 11

    g.dsu.init(g.vertices.size());
    g.initWorklist();

    g.bidirectedReach();

    if(g.calcNumReachablePairs() != 55){
        cout<<"fully connected graph (with only epsilon edges) has wrong number of reachable pairs"<<endl;
        return false;
    }
    return true;
}

bool flattenReachAndFlattenThenReachOnBracketIsSame(){
    //testing that flattenReach on brackets and flatten then reach on brackets yields same result
    int height = 3;
    graph g1 = buildSimple(height);
    graph g2 = g1.copy();

    g1.flattenReach("[");

    
    g2 = g2.flatten("[", height+1);
    g2.bidirectedReach();

    int numg1 = g1.calcNumReachablePairs();
    int numg2 = g2.calcNumReachablePairs();
    if(numg1 != numg2){
        cout<<"number of reachable pairs is different when doing flattenReach ("<<numg1<<") and when doing flatten, then reach ("<<numg2<<") on brackets"<<endl;
        return false;
    }
    return true;
}

bool flattenReachOnBracketOrParentheisIsSameForSimpleInterDyck(){
    //testing that flattenReach on brackets and parentheses should be the same for this simple case
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
    return true;
}

bool all4WaysGiveSameResultForRandomGraphs(){
    //testing for random graphs that all 4 different techniques produce same results
    int flattenHeight = 100;
    int edges = 20;
    int vertices = edges * 7 / 10;
    int repetitions = 20;
    for(int seed = 0; seed < repetitions; seed++){
        graph orig = Test::makeRandomGraph(seed, edges, vertices);

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

            cout<<"Number of reachable pairs should be the same for all 4 methods!\\\\"<<endl;
            cout<<"flattenReach on parenthesis: "<<num1<<"\\\\"<<endl;
            g1.printDetailReach();
            cout<<"flattenReach on bracket: "<<num2<<"\\\\"<<endl;
            g2.printDetailReach();
            cout<<"flatten, then reach on parenthesis up to height "<<flattenHeight<<": "<<num3<<"\\\\"<<endl;
            g3.printDetailReach();
            cout<<"flatten, then reach on brackets up to height "<<flattenHeight<<": "<<num3<<"\\\\"<<endl;
            g4.printDetailReach();

            cout<<"original number of vertices: "<<orig.vertices.size()<<"\\\\"<<endl;

            orig = orig.flatten("[", 8);
            orig.printGraphAsTikz();

            cout<<"seed is "<<seed<<endl;
            return false;
        }else{
            /*cout<<"we did it?? nums all = "<<num1<<endl;
            orig.printGraphAsTikz();
            orig = orig.flatten("[", 8);
            orig.printGraphAsTikz();*/
        }
    }
    return true;
}

bool Test::test(){

    if(!iterateOverEdgesGivesCorrectOrder()) return false;
    
    if(!oneOpenOneCloseParenthesisHas1pair()) return false;

    if(!smallInterDyckWorksForFlattenThenReach()) return false;
    
    if(!copyMakesIdenticalGraph()) return false;
    
    if(!simpleInterDyckOnFlattenThenReachForBothIsSame()) return false;

    if(!noEdgesMeansNoPairs()) return false;

    if(!fullyConnectedEpsEdgeGraphHasMaxPairs()) return false;
    
    if(!flattenReachAndFlattenThenReachOnBracketIsSame()) return false;

    if(!flattenReachOnBracketOrParentheisIsSameForSimpleInterDyck()) return false;

    if(!all4WaysGiveSameResultForRandomGraphs()) return false;

    return true;
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
