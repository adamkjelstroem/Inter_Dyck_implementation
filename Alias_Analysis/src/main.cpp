#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>

string test_cases[] = {
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

void d1dk_experiment(string s, string counterSymbol){
	graph g;
	g.constructFromDot("./spg/orig_bench/" + s + ".dot", counterSymbol == "[", counterSymbol == "("); 
	g.initWorklist();

	int n = g.N;

	graph g_copy = g.copy();
	g_copy.initWorklist();
	g_copy.bidirectedReach();
	int d_sccs = g_copy.computeSCCs().size();
	int d_reachable_pairs = g_copy.calcNumReachablePairs();

	clock_t t = clock();	
	
	g.bidirectedInterleavedDkD1Reach(s);

	int id_sccs = g.computeSCCs().size();
	int id_reachable_pairs = g.calcNumReachablePairs();

	g.deleteVertices();

	float time = ((float)clock()-t)/CLOCKS_PER_SEC;

	cout<<s<<" & "<<n<<" & "<<id_sccs<<" & "<<id_reachable_pairs<<" & "<<d_sccs<<" & "<<d_reachable_pairs<<" & "<<time<<" \\\\ \\hline"<<endl;
}

set<pair<int, int>> getReachablePairs(graph& g){
	set<pair<int, int>> pairs;
	for(int a = 0; a < g.N; a++){
		for(int b = a+1; b < g.N; b++){
			if(g.dsu.root(a) == g.dsu.root(b)){
				pair<int, int> p (a, b);
				pairs.insert(p);
			}
		}
	}
	return pairs;
}

void full_d1d1_experiment(){
	
		cout<<"\\begin{table}[]"<<endl;
		cout<<"\\begin{tabular}{|l|l|l|l|l|l|l|}"<<endl;
		cout<<"\\hline"<<endl;
		cout<<"Benchmark & N & ID-CCs & D-CCs & Time (s) \\\\ \\hline"<<endl;
	
		for(string s : test_cases){
			
			string s2 = "./spg/orig_bench/" + s + ".dot";
			
			//initialize graph
			graph* g = new graph;
			g->constructFromDot(s2, true, true);
			g->dsu.init(g->N);
			g->initWorklist();
		
			int n = g->N;
			
			graph g_copy = g->copy();
			g_copy.initWorklist();
			g_copy.bidirectedReach();
			int d_ccs = g_copy.computeSCCs().size();

			
			//timing logic
			clock_t t = clock();
			
			g->bidirectedInterleavedD1D1Reach();

			float time = ((float)clock()-t)/CLOCKS_PER_SEC;

			int id_ccs = g->computeSCCs().size();

			g->deleteVertices();

			delete g;

			cout<<s<<" & "<<n<<" & "<<id_ccs<<" & "<<d_ccs<<" & "<<time<<" \\\\ \\hline"<<endl;
		}

		cout<<"\\end{tabular}"<<endl;
		cout<<"\\end{table}"<<endl;
}

void full_union_dyck_of_d1d1_experiment(){
	for(auto s : test_cases){
		string s2 = "./spg/orig_bench/" + s + ".dot";

		graph d1d1;
		d1d1.constructFromDot(s2, true, true);
		d1d1.dsu.init(d1d1.N);
		d1d1.initWorklist();
		
		d1d1.bidirectedReach();

		cout<<"Number of D-SCCs for "<<s<<": "<<d1d1.computeSCCs().size()<<endl;
	}
}

void full_set_difference_experiment(){
	string flatten_label = "(";
	cout<<"Computing set differences between DkD1 and D1D1 when flattening DkD1 on "<<flatten_label<<endl;
	for(string s : test_cases){
		string s2 = "./spg/orig_bench/" + s + ".dot";

		//initialize d1d1 graph
		graph d1d1;
		d1d1.constructFromDot(s2, true, true);
		d1d1.dsu.init(d1d1.N);
		d1d1.initWorklist();

		//d1d1.printAsDot();

		//initialize dkd1 graph
		graph dkd1;
		dkd1.constructFromDot(s2, flatten_label == "[", flatten_label == "("); 
		dkd1.dsu.init(dkd1.N);
		dkd1.initWorklist();

		d1d1.bidirectedInterleavedD1D1Reach();

		dkd1.bidirectedInterleavedDkD1Reach(flatten_label);

		//compare results.

		//We extract pairs into list
		set<pair<int, int>> d1d1_pairs = getReachablePairs(d1d1);
		set<pair<int, int>> dkd1_pairs = getReachablePairs(dkd1);
		
		//since DkD1 pairs are a subset of D1D1 pairs,
		//we compute D1D1 pairs - DkD1 pairs

		int res = 0;
		for(auto d1d1pair : d1d1_pairs){
			//a pair is in the set difference if it is in d1d1 but not in dkd1
			if(dkd1_pairs.find(d1d1pair) == dkd1_pairs.end()){
				res++;
			}
		}
		cout<<s<<" - D1D1 pairs - DkD1 pairs: "<<res<<endl;
		cout<<"# d1d1 pairs : "<<d1d1_pairs.size()<<" - #dkd1 pairs : "<<dkd1_pairs.size()<<endl;
	}
}

void full_d1dk_experiment(){
	//Test setup for D1 dot Dk flattened to a bound=n
	
	string flatten_label = "(";

	cout<<"Table when flattening on "<<flatten_label<<endl;
	cout<<endl;

	cout<<"\\begin{table}[]"<<endl;
	cout<<"\\begin{tabular}{|l|l|l|l|l|l|l|}"<<endl;
	cout<<"\\hline"<<endl;
	cout<<"Benchmark & N & ID-SCCs & ID reachable pairs & D-SCCs & D reachable pairs & Time (s) \\\\ \\hline"<<endl;

	for(string s : test_cases){
		d1dk_experiment(s, flatten_label);
	}

	cout<<"\\end{tabular}"<<endl;
	cout<<"\\end{table}"<<endl;
}

void full_early_stopping_experiment(){
	for(string s : test_cases){
		string flatten_on = "(";
		
		int height = 100;

		
		string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
		s2 = "./spg/orig_bench/" + s + ".dot"; 
		graph g;
		g.constructFromDot(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
		//g.flattenReachRemade(flatten_on);
		
		cout<<"flattening graph "<<s<<"_reduced up to height "<<height<<endl;

		
		graph h = g.flatten(flatten_on, height);
		h.bidirectedReach();
		
		int low_rep = -1, high_rep = -1;

		bool possible = false;
				
		for (int hb = 0; hb < height-2; hb++){
			for(int hb2 = hb+1; hb2 < height-2; hb2++){
				possible = true;
				for(int i = 0; i < g.N; i++){
					int x = g.vertices[i]->x;
					Vertex* h1_root_vertex = h.vertices[h.dsu.root(h.getVertex(x, hb, "")->id)];
					Vertex* h2_root_vertex = h.vertices[h.dsu.root(h.getVertex(x, hb2, "")->id)];
					if(h1_root_vertex->x != h2_root_vertex->x){
						possible = false;
					}
				}
				if(possible){
					cout<<"We found an example: "<<hb<<", "<<hb2<<endl;
					low_rep = hb;
					high_rep = hb2;
					break;
				}
			}
			if(possible) break;
		}

		for(int j = 1; j < 10; j++){
			possible = true;
			for(int i = 0; i < g.N; i++){
				int x = g.vertices[i]->x;
				Vertex* h1_root_vertex = h.vertices[h.dsu.root(h.getVertex(x,  low_rep + 1, "")->id)];
				Vertex* h2_root_vertex = h.vertices[h.dsu.root(h.getVertex(x, high_rep + 1, "")->id)];
				if(h1_root_vertex->x != h2_root_vertex->x){
					possible = false;
				}
			}
			cout<<"Layer "<<low_rep + j<<" and layer "<<high_rep + j<<" match, too : "<<possible<<endl;
			if(!possible){
				cout<<"ERROR"<<endl;
				return;
			}
		}


		graph h2 = g.flatten(flatten_on, high_rep + 1);

		h2.bidirectedReach();

		int a = h.calcNumReachablePairs();
		int b = h2.calcNumReachablePairs();
		cout<<"number of pairs when flattening up to "<<height<<": "<<a<<endl;
		cout<<"number of pairs when flattening up to "<<(high_rep + 1)<<": "<<b<<endl;
		if(a == b){
			break;
		}

	}
}

int main(int argc, const char * argv[]){
	//if(argc!=2){
	//	cerr<<"the argument should be path to file containing spg graph"<<endl;
	//	return 1;
	//}

	//full_d1d1_experiment();

	//full_union_dyck_experiment();

	//full_set_difference_experiment();

	//full_d1dk_experiment();

	//full_early_stopping_experiment();

}