#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <tuple>

string getPathOf(string benchmark){
	return "./benchmarks/" + benchmark + ".dot";
}

tuple<int, int, int, float> dkd1_experiment(string s, string counterSymbol){
	graph g;
	g.constructFromDot(getPathOf(s), counterSymbol == "[", counterSymbol == "("); 
	g.initWorklist();

	int n = g.N;

	graph g_copy = g.copy();
	g_copy.initWorklist();
	g_copy.bidirectedReach();
	int d_ccs = g_copy.computeSCCs().size();
	int d_reachable_pairs = g_copy.calcNumReachablePairs();

	clock_t t = clock();	
	
	g.bidirectedInterleavedDkD1Reach(s);

	int id_ccs = g.computeSCCs().size();
	int id_reachable_pairs = g.calcNumReachablePairs();

	g.deleteVertices();

	float time = ((float)clock()-t)/CLOCKS_PER_SEC;

	cout<<" "<<n<<", "<<id_ccs<<", "<<d_ccs<<", "<<time<<endl;

	return std::make_tuple(n, id_ccs, d_ccs, time);
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

tuple<int, int, int, float> d1d1_experiment(string s){
	//initialize graph
	graph* g = new graph;
	g->constructFromDot(getPathOf(s), true, true);
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

	return std::make_tuple(n, id_ccs, d_ccs, time);
}

void full_d1d1_experiment(string test_cases[], int n_cases){
	
	cout<<"%Table for D1 odot D1 experiment:"<<endl;
	cout<<"\\begin{table}[]"<<endl;
	cout<<"\\begin{tabular}{|l|l|l|l|l|}"<<endl;
	cout<<"\\hline"<<endl;
	cout<<"Benchmark & N & ID-CCs & D-CCs & Time (s) \\\\ \\hline"<<endl;

	for(int i = 0; i < n_cases; i++){
		string s = test_cases[i];	
		
		int n, id_ccs, d_ccs, time;
		tie(n, id_ccs, d_ccs, time) = d1d1_experiment(s);
		
		cout<<s<<" & "<<n<<" & "<<id_ccs<<" & "<<d_ccs<<" & "<<time<<" \\\\ \\hline"<<endl;
	}

	cout<<"\\end{tabular}"<<endl;
	cout<<"\\end{table}"<<endl;
}

void full_set_difference_experiment(string test_cases[], int n_cases){
	string flatten_label = "(";
	cout<<"Computing set differences between DkD1 and D1D1 when flattening DkD1 on "<<flatten_label<<endl;
	for(int i = 0; i < n_cases; i++){
		string s = test_cases[i];
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

void full_dkd1_experiment(string test_cases[], int n_cases){
	//Test setup for D1 dot Dk flattened to a bound=n
	
	string flatten_label = "(";

	cout<<endl;
	cout<<"%Table when flattening on '"<<flatten_label<<"':"<<endl;
	cout<<"\\begin{table}[]"<<endl;
	cout<<"\\begin{tabular}{|l|l|l|l|l|}"<<endl;
	cout<<"\\hline"<<endl;
	cout<<"Benchmark & N & ID-CCs & D-CCs & Time (s) \\\\ \\hline"<<endl;

	for(int i = 0; i < n_cases; i++){
		string s = test_cases[i];
		int n, id_ccs, d_ccs, time;
		tie(n, id_ccs, d_ccs, time) = dkd1_experiment(s, flatten_label);
		cout<<s<<" & "<<n<<" & "<<id_ccs<<" & "<<d_ccs<<" & "<<time<<" \\\\ \\hline"<<endl;
	}

	cout<<"\\end{tabular}"<<endl;
	cout<<"\\end{table}"<<endl;
}

void full_early_stopping_experiment(string test_cases[], int n_cases){
	for(int i = 0; i < n_cases; i++){
		string s = test_cases[i];
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

void full_both_experiment(string test_cases[], int n_cases){
	
	cout<<"\\begin{table}[]"<<endl;
	cout<<"\\label{tab:results_merged}"<<endl;
	cout<<"\\caption{"<<endl;
	cout<<"Experimental results on $\\Dyck_1\\odot \\Dyck_1$ and $\\Dyck_k\\odot \\Dyck_1$ reachability."<<endl;
	cout<<"In each case, ID-CCs denotes the number of connected components wrt the interleaved Dyck language,"<<endl;
	cout<<"while D-CCs denotes the number of connected components wrt the under-approximating Dyck language on the union alphabet."<<endl;
	cout<<"%$n$ denotes the number of nodes in the input graph."<<endl;
	cout<<"}"<<endl;
	cout<<"\\begin{tabular}{| c | c || c | c | c || c | c | c |}"<<endl;
	cout<<"\\hline"<<endl;
	cout<<"\\textbf{Benchmark}  & \\textbf{$n$} & \\multicolumn{3}{c||}{$\\Dyck_1 \\odot \\Dyck_1$} & \\multicolumn{3}{c|}{$\\Dyck_k \\odot \\Dyck_1$} \\\\ \\hline"<<endl;
	cout<<"& & \\textbf{ID-CCs} & \\textbf{D-CCs} & \\textbf{Time (s)} & \\textbf{ID-CCs} & \\textbf{D-CCs} & \\textbf{Time (s)} \\\\ \\hline \\hline"<<endl;
	
	for(int i = 0; i < n_cases; i++){
		string s = test_cases[i];	
		
		int n, d1d1_id_ccs, d1d1_d_ccs, dkd1_id_ccs, dkd1_d_ccs;
		float d1d1_time, dkd1_time;

		tie(n, d1d1_id_ccs, d1d1_d_ccs, d1d1_time) = d1d1_experiment(s);

		tie(n, dkd1_id_ccs, dkd1_d_ccs, dkd1_time) = dkd1_experiment(s, "(");


		cout<<s<<" & "<<n<<" & "<<d1d1_id_ccs<<" & "<<d1d1_d_ccs<<" & "<<d1d1_time<<
				 " & "<<dkd1_id_ccs<<" & "<<dkd1_d_ccs<<" & "<<dkd1_time<<"\\\\ \\hline"<<endl;		
	}

	cout<<"\\end{tabular}"<<endl;
	cout<<"\\end{table}"<<endl;

}

int main(int argc, const char * argv[]){
	string *test_cases;
	int n_cases;
	
	if (argc >= 3 && strcmp(argv[1], "-b") == 0) {
		test_cases = new string[argc-2];
		n_cases = argc-2;
		for(int i = 2; i < argc; i++){
			test_cases[i-2] = argv[i];
		}
	}else{
		n_cases = 11;
		test_cases = new string[11];
		test_cases[0] = "antlr";
		test_cases[1] = "bloat";
		test_cases[2] = "chart";
		test_cases[3] = "eclipse";
		test_cases[4] = "fop";
		test_cases[5] = "hsqldb";
		test_cases[6] = "jython";
		test_cases[7] = "luindex";
		test_cases[8] = "lusearch";
		test_cases[9] = "pmd";
		test_cases[10]= "xalan";
	}

	//full_d1d1_experiment(test_cases, n_cases);

	full_dkd1_experiment(test_cases, n_cases);

	//full_both_experiment(test_cases, n_cases);

}