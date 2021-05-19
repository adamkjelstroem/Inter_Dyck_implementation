#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>


int main(int argc, const char * argv[]){
	if(argc!=2){
		cerr<<"the argument should be path to file containing spg graph"<<endl;
		return 1;
	}
	// Bidirected Reach Algorithm
	graph g2, g;
	clock_t time;
	struct timeval tv1,tv2;
	
	//TODO uncomment

	//true -> construct form input
	//false -> generate
	if(false){
		g.construct2(argv[1]); //NOTE using construct2 uses the new format
		g2 = g.copy();
		cout<<"Graph loaded from file"<<endl;
	}else{
		gettimeofday(&tv1, NULL);
		time = clock();
		srand((int)tv2.tv_usec);

		int edges = 10;
		int vertices = edges * 7 / 10;

		for(int i = 0; i < edges; i++){
			string a = to_string(rand() % vertices);
			string b = to_string(rand() % vertices);
			string field = "[";
			if(rand() % 2 == 0) field = "(";
			
			g.addedge(g.getVertex(a), g.getVertex(b), g.getfield(field));
		}
		
		g.dsu.init(g.vertices.size());
		g2 = g.copy();
	}
	
	
	

	gettimeofday(&tv1, NULL);
	time = clock();

	//true -> flattenReach
	//false -> flatten, then reach
	if(true){
		g.initWorklist();
		g2.initWorklist();

		g.flattenReach("[");
		g2.flattenReach("(");
	}else{
		g = g.flatten("[", 8);
		g2 = g2.flatten("(", 8);

		g.initWorklist();
		g2.initWorklist();

		g.bidirectedReach();
		g2.bidirectedReach();
	}
	

	time = clock() - time;
	gettimeofday(&tv2, NULL);

	//g.printDetailReach();

	cout<<"flattened on '[':\\\\"<<endl;
	g.printNumReachablePairs();
	cout<<"flattened on '(':\\\\"<<endl;
	g2.printNumReachablePairs();

	// time required for bidirectedReach
	cout<<"\nBidirected Reach Algorithm"<<endl;
	double bidir_reach_time_s = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
	cout<<"\tTime recorded in seconds : "<<bidir_reach_time_s<<"\n\n";

	cout<<endl<<endl;
	

	//TODO same here!
	//this code has been disabled
	/*

	// previous dyck Reachability Algorithm
	cout<<"previous existing Algorithm"<<endl;
	Ngraph ng;   // 
	clock_t ntime;
	struct timeval ntv1,ntv2;
	ng.construct(argv[1]);
	ng.initWorklist();

	gettimeofday(&ntv1,NULL);
	ntime = clock();
	ng.dyck_reach();
	ntime = clock()-ntime;
	gettimeofday(&ntv2,NULL);


	// time required for dyckReach
	cout<<"\tTime recorded in seconds : "<<(double) (ntv2.tv_usec - ntv1.tv_usec) / 1000000 +(double) (ntv2.tv_sec - ntv1.tv_sec)<<endl;
	*/

}