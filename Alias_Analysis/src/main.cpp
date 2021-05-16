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
	
	
	g2.construct2(argv[1]); //NOTE using construct2 uses the new format
	

	g2.flattenReach();
	
	if(true)return 0;
	
	struct timeval buildt1, buildt2;
	gettimeofday(&buildt1, NULL);
	time = clock();
	g = g2.flattenbracket(6); //TODO hardcoded
	time = clock() - time;
	gettimeofday(&buildt2, NULL);
	cout<<"\nFlattening Algorithm"<<endl;
	double flatten_time_s = (double) (buildt2.tv_usec - buildt1.tv_usec) / 1000000 +(double) (buildt2.tv_sec - buildt1.tv_sec);
	cout<<"\tTime recorded in seconds : "<<flatten_time_s<<"\n\n";

	g.initWorklist();


	g.printFlattenedGraphAsTikz();

	
	gettimeofday(&tv1,NULL);
	time = clock();
	g.bidirectedReach();
	time = clock()-time;
	gettimeofday(&tv2,NULL);	

	g.printDetaiLReachInterDyck(); //NOTE uses "InterDyckReach"

	// time required for bidirectedReach
	cout<<"\nBidirected Reach Algorithm"<<endl;
	double bidir_reach_time_s = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
	cout<<"\tTime recorded in seconds : "<<bidir_reach_time_s<<"\n\n";

	cout<<"\nTotal running time"<<endl;
	cout<<"\tTime recorded in seconds : "<<bidir_reach_time_s + flatten_time_s<<"\n\n";

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