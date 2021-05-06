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
	graph g;
	clock_t time;
	struct timeval tv1,tv2;
	g.construct(argv[1]);
	g.initWorklist();
	
	gettimeofday(&tv1,NULL);
	time = clock();
	g.bidirectedReach();
	time = clock()-time;
	gettimeofday(&tv2,NULL);	

	g.printReach();

	// time required for bidirectedReach
	cout<<"\nBidirected Reach Algorithm"<<endl;
	cout<<"\tTime recorded in seconds : "<<(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec)<<"\n\n";

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
}