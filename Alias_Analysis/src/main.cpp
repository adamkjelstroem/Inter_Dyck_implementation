#include "graph/graph.h"
#include "test/test.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>


int main(int argc, const char * argv[]){
	if (false) //TODO remove
	if(argc!=2){
		cerr<<"the argument should be path to file containing spg graph"<<endl;
		return 1;
	}

	if(false){
		string data[] = {
			"antlr",
			"bloat",
			"chart",
			"eclipse",
			"fop",
			"hsqldb",
			"jython",
			"luindex",
			"lusearch",
			"pmd",
			"xalan"
		};

		for(string s : data){
			//string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			string s2 = "./spg/orig_bench/" + s + ".dot";
			graph g;
			g.construct2(s2, true, true); //Parses files in the ".dot" format
			g.initWorklist();
			g.bidirectedReach();
			cout<<"D' reachability for original version of "<<s<<": "<<g.calcNumReachablePairs()<<endl;
		}
		return 0;
	}

	if(false){
		Test t;
		graph g = t.buildSimple(3);

		string flatten_label = "[";

		graph flattened = g.flatten(flatten_label, 2);

		flattened.printGraphAsTikz();

		flattened.bidirectedReach();

		flattened.printDetailReach();


		auto addLayer = [](Vertex a, Vertex b, field f, void* extra[]) {
				//TODO possibly move this definition outside the loop
				graph* flattened = (graph*)extra[0];
				int layer = *((int*)extra[1]);
				string field_name = *(string*)extra[2];

				if(f.field_name == field_name){ //TODO cache id of "[" field and do comparison on
					//flatten on this field
					flattened->addEdge(
						a.x, layer-1, 
						b.x, layer, 
						"eps"
					);
				}else{
					flattened->addEdge(
						a.x, layer,
						b.x, layer,
						f.field_name
					);
						
				}
			};
		
		int i = 2;
		void* w[] = {&flattened, &i, &flatten_label};
		g.iterateOverEdges(addLayer, w);

		flattened.dsu.init(flattened.N); 

		flattened.initWorklist();

		flattened.printGraphAsTikz();



		flattened.bidirectedReach();

		flattened.printDetailReach();

		{
			i = 3;
			void* w[] = {&flattened, &i, &flatten_label};
			g.iterateOverEdges(addLayer, w);
		}

		flattened.dsu.init(flattened.N); 

		flattened.initWorklist();

		flattened.printGraphAsTikz();


		flattened.bidirectedReach();

		flattened.printDetailReach();

		return 0;
	}

	if(true){
		string data[] = {
			"antlr",
			"bloat",
			"chart",
			"eclipse",
			"fop",
			"hsqldb",
			"jython",
			"luindex",
			"lusearch",
			"pmd",
			"xalan"
		};

		string flatten_on = "[";
		for(string s : data){
			string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			graph g;
			
			cout<<"running heuristicReductionBeforeFlattenReach on "<<s<<endl;

			g.construct2(s2, true, true); //Parses files in the ".dot" format as D1 dot D1


			clock_t time;
			struct timeval tv1,tv2;


			gettimeofday(&tv1, NULL);
			time = clock();

			g.heuristicReductionBeforeFlattenReach(flatten_on);


			time = clock() - time;
			gettimeofday(&tv2, NULL);

			cout<<"D1 dot D1 reachability for "<<s<<"_reduced (when flattening on '"<<flatten_on<<"'): "<<g.calcNumReachablePairs()<<endl;


			double bidir_reach_time_s = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
			cout<<"\tTime recorded in seconds : "<<bidir_reach_time_s<<"\n\n";
			
		}
		return 0;
	}

	if(true){
		string data[] = {
			"antlr",
			"bloat",
			"chart",
			"eclipse",
			"fop",
			"hsqldb",
			"jython",
			"luindex",
			"lusearch",
			"pmd",
			"xalan"
		};

		for(string s : data){
			string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			graph g;
			
			g.construct2(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
			long long n = g.N;
			long long c = n*n*18 + 6*n;
			cout<<"Nodes of graph "<<s<<"_reduced: "<<n<<". This yields "<<c<<" layers with a total of "<<c*n<<" nodes when flattened."<<endl;
		}

		return 0;
	}

	if(false){
		string data[] = {
			"antlr",
			"bloat",
			"chart",
			"eclipse",
			"fop",
			"hsqldb",
			"jython",
			"luindex",
			"lusearch",
			"pmd",
			"xalan"
		};
		string flatten_on = "[";
		for(string s : data){
			string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			graph g;
			
			g.construct2(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
			//g.flattenReachRemade(flatten_on);
			
			int height = (int)(((long long) 1000*1000*5) / ((long long)g.N));

			cout<<"flattening graph "<<s<<" up to height "<<height<<endl;

			graph h = g.flatten(flatten_on, height);

			h.bidirectedReach();

			cout<<"D1 dot D1 reachability for "<<s<<"_reduced (when flattening on '"<<flatten_on<<"' up to height "<<height<<"): "<<h.calcNumReachablePairs()<<endl;
		}
		return 0;
	}

	/*
	if(true){
		{
			graph g;
			g.construct2(argv[1]); //NOTE using construct2 uses the new format
			g.initWorklist();
			g.bidirectedReach();
			cout<<"D' reachability: "<<g.calcNumReachablePairs()<<endl;
		}
		
		graph g;
		g.construct2(argv[1]); //NOTE using construct2 uses the new format
		

		for(int i = 0; i < 5; i++){
			cout<<"doing iteration "<<(i+1)<<" of 10"<<endl;
			g.flattenReach2("(");
			cout<<"resulting number of pairs: "<<g.calcNumReachablePairs()<<endl;
		}

		return 0;
	}*/

	if(true){	
		Test t;
		bool res = t.test();

		if(!res){
			cout<<"A test failed!"<<endl;
		}else{
			cout<<"All tests passed!"<<endl;
		}
		return 0;
	}	

	while (false){

		//compute number of reachable pairs for many graphs using
		//1) flatten on parenthesis, then compute
		//2) flatten on brackets, then compute
		//3) flattenReach on parenthesis
		//4) flattenReach on brackets
		// if these don't all match, print the original graph
		
		graph original, g1, g2, g3, g4;

		clock_t time;
		struct timeval tv1;

		gettimeofday(&tv1, NULL);
		time = clock();
		
		original = Test::makeRandomGraph((int)tv1.tv_usec, 10, 7);

		g1 = original.copy();
		g2 = original.copy();
		g3 = original.copy();
		g4 = original.copy();

		g1 = g1.flatten("(", 20);
		g1.bidirectedReach();
		int g1Pairs = g1.calcNumReachablePairs();
		cout<<"g1 done"<<endl;

		g2 = g2.flatten("[", 20);
		g2.bidirectedReach();
		int g2Pairs = g2.calcNumReachablePairs();
		cout<<"g2 done"<<endl;

		g3.flattenReach("(");
		int g3Pairs = g3.calcNumReachablePairs();
		cout<<"g3 done"<<endl;

		g4.flattenReach("[");
		int g4Pairs = g4.calcNumReachablePairs();
		cout<<"g4 done"<<endl;

		if(!(g1Pairs == g2Pairs && g2Pairs == g3Pairs && g3Pairs == g4Pairs)){


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
			cout<<"flattenReach on (: "<<g3Pairs<<endl;
			cout<<"flattenReach on [: "<<g4Pairs<<endl;
			original.printAsDot();
			

			return 0;
		}

	}
	
	
	// Bidirected Reach Algorithm
	graph g2, g;
	clock_t time;
	struct timeval tv1,tv2;
	

	//true -> construct form input
	//false -> generate
	if(true){
		g.construct2(argv[1], true, true); //NOTE using construct2 uses the new format
		g2 = g.copy();
		cout<<"Graph loaded from file"<<endl;
	}else{
		gettimeofday(&tv1, NULL);
		time = clock();
		
		g = Test::makeRandomGraph((int)tv1.tv_usec, 10, 7);

		g2 = g.copy();
		cout<<"Graph generated"<<endl;
	}


	gettimeofday(&tv1, NULL);
	time = clock();

	//true -> flattenReach
	//false -> flatten, then reach
	if(true){
		g.flattenReach("[");
		g2.flattenReach("(");
	}else{
		g = g.flatten("[", 80);
		//g2 = g2.flatten("(", 8);

		//g.printGraphAsTikz();
		//g2.printGraphAsTikz();

		g.bidirectedReach();
		//g2.bidirectedReach();
	}
	

	time = clock() - time;
	gettimeofday(&tv2, NULL);

	//g.printDetailReach();

	cout<<"flattened on '[':\\\\"<<endl;
	cout<<"Number of reachable pairs: "<<g.calcNumReachablePairs()<<endl;
	//g.printDetailReach();
	cout<<"flattened on '(':\\\\"<<endl;
	cout<<"Number of reachable pairs: "<<g2.calcNumReachablePairs()<<endl;
	//g2.printDetailReach();

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