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
	for(string s : test_cases){
		string s2 = "./spg/orig_bench/" + s + ".dot";

		//initialize d1d1 graph
		graph d1d1;
		d1d1.constructFromDot(s2, true, true);
		d1d1.dsu.init(d1d1.N);
		d1d1.initWorklist();
		
		clock_t t = clock();
		
		d1d1.bidirectedInterleavedD1D1Reach();

		float time = ((float)clock()-t)/CLOCKS_PER_SEC;

		cout<<"Number of ID-SCCs for "<<s<<": "<<d1d1.computeSCCs().size()<<endl;
		cout<<"Time: "<<time<<" seconds"<<endl;
	}
}

void full_union_dyck_experiment(){
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

int main(int argc, const char * argv[]){
	//if(argc!=2){
	//	cerr<<"the argument should be path to file containing spg graph"<<endl;
	//	return 1;
	//}

	//full_d1d1_experiment();

	full_union_dyck_experiment();

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

		for(auto s : data){
			string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";

			graph d1d1;
			d1d1.constructFromDot(s2, false, false);
			d1d1.dsu.init(d1d1.N);
			d1d1.initWorklist();
			
			d1d1.bidirectedReach();

			d1d1 = d1d1.makeCopyWithoutDuplicates();

			auto d_sccs = d1d1.computeDisjointSets();

			int root_of_biggest = -1;
			int max = 0;
			for(auto el : d_sccs){
				if(root_of_biggest == -1 || el.second.size() > max){
					max = el.second.size();
					root_of_biggest = el.first;
				}
			}


			graph subgraph = d1d1.buildSubgraph(d_sccs[root_of_biggest]);



			//do trimming on subgraph

			subgraph = subgraph.trim_dkd1(subgraph);

			auto dsccs2 = subgraph.computeDisjointSets();

			for(auto el : dsccs2){
				graph g = subgraph.buildSubgraph(el.second);
				bool brackets = false, parentheses = false;
				for(Vertex* v : g.vertices){
					for(auto e : v->edges){
						if(e.first.field_name.find("(") != std::string::npos){
							parentheses = true;
						}
						if(e.first.field_name.find("[") != std::string::npos){
							brackets = true;
						}
					}
				}
				if(parentheses && brackets){

					cout<<"First visualization (before trimming): "<<endl;

					subgraph.printAsDot();

					cout<<"Second visualization (after trimming): "<<endl;

					subgraph.printAsDot();
				}
			}



		}
		return 0;
	}


	return 0;

	if(false){
		//test for set difference
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

		string flatten_label = "(";
		cout<<"Computing set differences between DkD1 and D1D1 when flattening DkD1 on "<<flatten_label<<endl;
		for(string s : data){
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

		return 0;
	}


	if(true){
		//Test setup for D1 dot Dk flattened to a bound=n
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

		string flatten_label = "(";

		cout<<"Table when flattening on "<<flatten_label<<endl;
		cout<<endl;

		cout<<"\\begin{table}[]"<<endl;
		cout<<"\\begin{tabular}{|l|l|l|l|l|l|l|}"<<endl;
		cout<<"\\hline"<<endl;
		cout<<"Benchmark & N & ID-SCCs & ID reachable pairs & D-SCCs & D reachable pairs & Time (s) \\\\ \\hline"<<endl;
	
		for(string s : data){
			d1dk_experiment(s, flatten_label);
		}

		cout<<"\\end{tabular}"<<endl;
		cout<<"\\end{table}"<<endl;

		return 0;
	}

	if(false){
		//Procedure as of 28 june 2021, with print statements updated to export as tex file
		string benchmarks[] = {
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

		cout<<"\\begin{table}[]"<<endl;
		cout<<"\\begin{tabular}{|l|l|l|l|l|l|l|}"<<endl;
		cout<<"\\hline"<<endl;
		cout<<"Benchmark & N & ID-SCCs & ID reachable pairs & D-SCCs & D reachable pairs & Time (s) \\\\ \\hline"<<endl;
	
		for(string s : benchmarks){
			
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
			int d_sccs = g_copy.computeSCCs().size();
			int d_reachable_pairs = g_copy.calcNumReachablePairs();

			
			//timing logic
			clock_t t = clock();
			
			g->bidirectedInterleavedD1D1Reach();

			int id_sccs = g->computeSCCs().size();
			int id_reachable_pairs = g->calcNumReachablePairs();

			g->deleteVertices();
			delete g;

			float time = ((float)clock()-t)/CLOCKS_PER_SEC;

			cout<<s<<" & "<<n<<" & "<<id_sccs<<" & "<<id_reachable_pairs<<" & "<<d_sccs<<" & "<<d_reachable_pairs<<" & "<<time<<" \\\\ \\hline"<<endl;
		}

		cout<<"\\end{tabular}"<<endl;
		cout<<"\\end{table}"<<endl;

		return 0;
	}




	//TODO delete?
	if(false){
		//Procedure as of 28 june 2021
		string benchmarks[] = {
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

		//hyperparameters
		bool using_reduced = false;
		for(string s : benchmarks){
			
			string s2;
			if(using_reduced){
				s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			} else {
				s2 = "./spg/orig_bench/" + s + ".dot";
			}

			//initialize graph
			graph* g = new graph;
			g->constructFromDot(s2, true, true);
			g->dsu.init(g->N);
			g->initWorklist();
		

			cout<<endl;
			cout<<"-----------------"<<endl;
			cout<<endl;
			cout<<"Computing on '"<<s;
			if (using_reduced) cout<<"_reduced";
			cout<<"'"<<endl;
			cout<<endl;
			cout<<"Initial graph:"<<endl;
			g->printSparsenessFacts();

			{
				graph copy_ignored = g->copy_ignoring("[");
				copy_ignored.initWorklist();
				copy_ignored.bidirectedReach();
				cout<<endl;
				cout<<"Number of reachable pairs when replacing edges on one counter with epsilon edges: "<<copy_ignored.calcNumReachablePairs()<<endl;
				cout<<"Number of DSCCs when replacing edges on one counter with epsilon edges: "<<copy_ignored.computeSCCs().size()<<endl;
				copy_ignored.deleteVertices();
			}
			
			//timing logic
			clock_t t = clock();
			

			//Reduce graph via bidirected reach, as it is a sound under-approximation
			g->bidirectedReach();
			cout<<endl;
			cout<<"Number of reachable pairs w.r.t. (non-interleaved) bidirected dyck reachability: "<<g->calcNumReachablePairs()<<endl;
			cout<<"Number of DSCCs w.r.t. (non-interleaved) bidirected dyck reachability: "<<g->computeSCCs().size()<<endl;
			cout<<"Number of disjoint components: "<<g->computeDisjointSets().size()<<endl;

			graph g_working = g->makeCopyWithoutDuplicates(); //construct working copy of g without dublicate edges.
			cout<<endl;
			cout<<"Graph after reducing via (non-interleaved) bidirected dyck reachability: "<<endl;
			g_working.printSparsenessFacts();
			

			//use various rules of trimming to reduce graph
			g_working = g_working.trim_d1d1(g_working);
			cout<<endl;
			cout<<"Graph after trimming:"<<endl;
			g_working.printSparsenessFacts();


			//split graph into disjoint components
			map<int,set<int>> disjoint_components = g_working.computeDisjointSets();
			cout<<endl;
			cout<<"Found "<<disjoint_components.size()<<" disjoint components w.r.t. plain reachability. ";
			cout<<"Each is treated as its own graph."<<endl;
			
			//further split each component if possible, then flatten up to the bound and perform
			//calculations directly.
			for(auto s : disjoint_components){
				graph g_part = g_working.buildSubgraph(s.second);
				g_part.removeHubVertexIfExistsThenCalc(g_part, *g);
			}
			
			g_working.deleteVertices();

			cout<<"Number of reachable pairs in graph w.r.t. interleaved, bidirected dyck reachability: "<<g->calcNumReachablePairs()<<endl;
			cout<<"Number of DSCCs in graph w.r.t. interleaved, bidirected dyck reachability: "<<g->computeSCCs().size()<<endl;

			g->deleteVertices();
			delete g;

			cout<<"Total time: "<<((float)clock()-t)/CLOCKS_PER_SEC<<" s"<<endl;
		}

		return 0;
	}




	if(false){
		//Procedure as of 28 june 2021
		string benchmarks[] = {
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

		//hyperparameters
		bool using_reduced = false;
		int iterations = 3;

		

		for(string s : benchmarks){
			
			string s2;
			if(using_reduced){
				s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			} else {
				s2 = "./spg/orig_bench/" + s + ".dot";
			}

			//Data to work on
			graph* g = new graph;
			set<int> singletons;


			if(true){
				g->constructFromDot(s2, true, true);
				g->dsu.init(g->N);
				g->initWorklist();
			}else{
				g->addEdge(0,0,1,0,"[");
				g->addEdge(1,0,2,0,"(");
				g->addEdge(3,0,2,0,"[");
				g->addEdge(4,0,3,0,"(");
				g->dsu.init(g->N);
				g->initWorklist();
			}	

			cout<<endl;
			cout<<endl;
			cout<<"-----------------"<<endl;
			cout<<endl;
			cout<<"computing on "<<s;
			if (using_reduced) cout<<"_reduced";
			cout<<endl;
			cout<<"Original size of g: "<<g->N<<endl;
			cout<<"Running "<<iterations<<" iterations of pre-processing."<<endl;

			//dumb copy that works for pointers.
			graph* g_working = new graph;
			for(Vertex* v : g->vertices){
				for(auto edge : v->edges){
					for(auto u : edge.second){
						g_working->addEdge(
							g->vertices[u]->x, 0,
							v->x, 0,
							edge.first.field_name
						);
					}
				}
			}
			g_working->dsu.init(g_working->N);
			g_working->initWorklist();

			for(int i = 0; i < iterations; i++){
				cout<<endl;
				cout<<"Doing iteration "<<(i+1)<<endl;

				graph h = g->flatten("[", 10);
				h.initWorklist();
				h.bidirectedReach();

				//TODO calculate number of reachable pairs by constructing sccs
				if(true){
					//make copy of dsu in g

					//merge with new information from h

					//compute sccs

					//count reachable pairs

					//print reachable pairs
				}
				
				//cout<<"Number of reachable pairs thus far when flattening to 10: "<<h.calcNumReachablePairs()<<endl;

				//Timing logic
				clock_t t;
				t = clock();
				
				//this code deletes singletons
				int d1, d2;
				if(false){	
					graph g_ign_1 = g_working->copy_ignoring("[");	
					graph g_ign_2 = g_working->copy_ignoring("(");

					g_ign_1.initWorklist();
					g_ign_2.initWorklist();

					g_ign_1.bidirectedReach();
					g_ign_2.bidirectedReach();
 
					d1 = g_ign_1.calcNumReachablePairs();
					d2 = g_ign_2.calcNumReachablePairs();


					{
						set<int> singletons_x;

						auto scc_1 = g_ign_1.computeSCCs();
						for(auto scc : scc_1){
							if(scc.second.size() == 1) singletons_x.insert(g_ign_1.vertices[scc.first]->x);
						}
						auto scc_2 = g_ign_2.computeSCCs();
						for(auto scc : scc_2){
							if(scc.second.size() == 1) singletons_x.insert(g_ign_2.vertices[scc.first]->x);
						}
						//'singletons' now contains the ids in g of any node that is a singleton in at least one
						//of the 'ignore' graphs
						for (auto x : singletons_x){
							singletons.insert(g->getVertex(x, 0, "")->id);	
						}
					}

					g_ign_1.deleteVertices();
					g_ign_2.deleteVertices();

					graph* g_working_2 = new graph;

					//build working copy of g without the deleted singles edges
					for (Vertex* u : g_working->vertices){
						if(singletons.find(u->id) == singletons.end()){
							//u is not a singleton
							for (auto edge : u->edges){
								for(auto v_id : edge.second){
									if(singletons.find(v_id) == singletons.end()){
										//if v is not a singleton, either
										//then add an edge between them

										//(actually add it between their respective roots)
										g_working_2->addEdge(
											g_working->vertices[g_working->dsu.root(u->id)]->x, 
											0, 
											g_working->vertices[g_working->dsu.root(v_id)]->x, 
											0, edge.first.field_name);
									}
								}
							}
						}
						
					}


					g_working_2->dsu.init(g_working_2->N);
					g_working_2->initWorklist();

					//overwrite g_working with g_working_2
					g_working->deleteVertices();
					delete g_working;

					g_working = g_working_2;
				}


				cout<<"Size of g with singletons deleted: "<<g_working->N<<endl;

				if(true){
					int num = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() > 1){
								num += edge.second.size();
								//cout<<"Found example "<<u->x<<" with #edges of label "<<edge.first.field_name<<" equal to "<<edge.second.size()<<endl;
							}
						}
					}
					cout<<"repeating edges before: "<<num<<endl;
				}

				//construct replacement for g_working that is actually sparse
				{
					//do 2 loop-overs. the first time around, just find nodes with too many
					//outgoing edges, and merge them in g. 
					//Then in the second iteration, add the first edge whenever there are too many (and do so from the root in g to the root in g)
					for (Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() >= 2){
								//found example of vertex with too many edges
								
								//find id of first element in list
								int to_merge = g->getVertex(g_working->vertices[edge.second.front()]->x, 0, "")->id; 
								
								auto it = edge.second.begin();
								it++; //start at 2nd value of list
								while(it!=edge.second.end()){
									//find all others in g and make sure they're added to the same scc in g
									int other_to_merge = g->getVertex(g_working->vertices[*it]->x, 0, "")->id;
									if(g->dsu.root(other_to_merge) != g->dsu.root(to_merge)){
										//only merge if new information
										g->dsu.merge(
											g->dsu.root(to_merge),
											g->dsu.root(other_to_merge) //calling dsu.root() respects precondition on merge()
										);
									}

									it++;
								}
							}
						}
					}
					
					graph* g_working_2 = new graph;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() >= 2){
								//just add the first edge to g_working_2
								int g_working_v_x = g_working->vertices[edge.second.front()]->x;
								auto v_in_g = g->getVertex(g_working_v_x, 0, "")->id;
								auto root_of_v_in_g = g->dsu.root(v_in_g);
								auto x_of_root_of_v_in_g = g->vertices[root_of_v_in_g]->x;
					
								g_working_2->addEdge(
									//awkward edge flip here; add them in opposite order
									x_of_root_of_v_in_g, 0,
									g->vertices[g->dsu.root(g->getVertex(u->x, 0, "")->id)]->x, 0,
									edge.first.field_name
								);
							}
						}
					}
					g_working_2->dsu.init(g_working_2->N);
					g_working_2->initWorklist();

					//overwrite g_working with g_working_2
					g_working->deleteVertices();
					delete g_working;

					g_working = g_working_2;
				}

				//For now, just print if it is sparse
				if(true){
					int num = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() > 1){
								num++;
								//cout<<"Found example "<<u->x<<" with #edges of label "<<edge.first.field_name<<" equal to "<<edge.second.size()<<endl;
							}
						}
					}
					cout<<"repeating edges after: "<<num<<endl;
				}
				cout<<"size of g after merging for sparseness: "<<g_working->N<<endl;

				{
					int vertices = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							vertices += edge.second.size();
						}
					}
					cout<<"total vertices in graph: "<<vertices<<endl;
				}

				
				//output if g_working contains more than 1 scc wrt plain reachability
				int max = 0;
				{
					DSU dsu;
					dsu.init(g_working->N);
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							for (auto v_id : edge.second){
								if(dsu.root(u->id) != g->dsu.root(v_id)){
									dsu.merge(dsu.root(u->id), dsu.root(v_id));
								}
							}
						}
					}
					map<int,set<int>> scc;
					for(int i=0;i<g_working->N;i++){
						scc[dsu.root(i)].insert(i);
					}
					cout<<"Found "<<scc.size()<<" disjoint components w.r.t plain reachability"<<endl;
					for(auto el : scc){
						int alt = el.second.size();
						if(alt > max){
							max = alt;
						}
					}
					cout<<"Size of biggest such component: "<<max<<endl;	
				}
			}
			

			cout<<"NUMBER: "<<g->calcNumReachablePairs()<<endl;

			return 0;

			g_working->deleteVertices();
			delete g_working;

			g->deleteVertices();
			delete g;
		}

		return 0;
	}


	if(true){
		//Procedure as of 28 june 2021 (old)
		string benchmarks[] = {
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

		//hyperparameters
		bool using_reduced = false;
		int iterations = 3;

		

		for(string s : benchmarks){
			//Data to work on
			graph* g = new graph;
			set<int> singletons;

			string s2;
			if(using_reduced){
				s2 = "./spg/reduced_bench/" + s + "_reduced.dot";
			} else {
				s2 = "./spg/orig_bench/" + s + ".dot";
			}

			if(true){
				g->constructFromDot(s2, true, true);
				g->dsu.init(g->N);
				g->initWorklist();
			}else{
				g->addEdge(0,0,1,0,"[");
				g->addEdge(1,0,2,0,"(");
				g->addEdge(3,0,2,0,"[");
				g->addEdge(4,0,3,0,"(");
				g->dsu.init(g->N);
				g->initWorklist();
			}	

			cout<<endl;
			cout<<endl;
			cout<<"-----------------"<<endl;
			cout<<endl;
			cout<<"computing on "<<s;
			if (using_reduced) cout<<"_reduced";
			cout<<endl;
			cout<<"Original size of g: "<<g->N<<endl;

			{
				graph copy = g->copy();
				copy.initWorklist();
				copy.bidirectedReach();
				cout<<"Number of reachable pairs wrt [ and ( (non-interleaved): "<<copy.calcNumReachablePairs()<<endl;

				copy.deleteVertices();
			}

			cout<<"Running "<<iterations<<" iterations."<<endl;

			for(int i = 0; i < 3; i++){
				int height = 10 + i * 5; // we gradually increase height
				cout<<"Doing iteration "<<(i+1)<<" flattening to height "<<height<<endl;

				bool stillConnectingToNewLayer = true;

				int sccs_before_iteration = g->computeSCCs().size();
				int sccs_after_first_reduction = 0;
				
				//Timing logic
				clock_t t;
				t = clock();
				

				int d1, d2;
				int reachable_pairs_in_intersection = 0;
				{	
					graph g_ign_1 = g->copy_ignoring("[");	
					graph g_ign_2 = g->copy_ignoring("(");

					g_ign_1.initWorklist();
					g_ign_2.initWorklist();

					g_ign_1.bidirectedReach();
					g_ign_2.bidirectedReach();
 
					d1 = g_ign_1.calcNumReachablePairs();
					d2 = g_ign_2.calcNumReachablePairs();

					//Compute intersection of reachable pairs
					auto scc_1 = g_ign_1.computeSCCs();
					for(auto scc : scc_1){
						for (auto v1 : scc.second){
							for(auto v2 : scc.second){
								if(v2 > v1){ 
									//for every unique pair of vertices in the same scc in g_ign_1:

									//note: given how "copy_ignoring" works, we can safely assume that vertices have the same ids
									//in g_ign_1 and g_ign_2
									if(g_ign_2.dsu.root(v1) == g_ign_2.dsu.root(v2)){
										//if they're also in the same scc in the other graph:
										reachable_pairs_in_intersection++;
									}
								}
							}
						}
					}

					{
						set<int> singletons_x;
						for(auto scc : scc_1){
							if(scc.second.size() == 1) singletons_x.insert(g_ign_1.vertices[scc.first]->x);
						}
						auto scc_2 = g_ign_2.computeSCCs();
						for(auto scc : scc_2){
							if(scc.second.size() == 1) singletons_x.insert(g_ign_2.vertices[scc.first]->x);
						}
						//'singletons' now contains the ids in g of any node that is a singleton in at least one
						//of the 'ignore' graphs
						for (auto x : singletons_x){
							singletons.insert(g->getVertex(x, 0, "")->id);	
						}
					}

					g_ign_1.deleteVertices();
					g_ign_2.deleteVertices();
				}

				graph* g_working = new graph;

				//build working copy of g without the deleted singles edges
				for (Vertex* u : g->vertices){
					if(singletons.find(u->id) == singletons.end()){
						//u is not a singleton
						for (auto edge : u->edges){
							for(auto v_id : edge.second){
								if(singletons.find(v_id) == singletons.end()){
									//if v is not a singleton, either
									//then add an edge between them

									//(actually add it between their respective roots)
									g_working->addEdge(
										g->vertices[g->dsu.root(u->id)]->x, 
										0, 
										g->vertices[g->dsu.root(v_id)]->x, 
										0, edge.first.field_name);
								}
							}
						}
					}
					
				}

				g_working->dsu.init(g_working->N);
				g_working->initWorklist();

				cout<<"Size of g with singletons deleted: "<<g_working->N<<endl;

				{
					int num = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() > 1){
								num++;
								//cout<<"Found example "<<u->x<<" with #edges of label "<<edge.first.field_name<<" equal to "<<edge.second.size()<<endl;
							}
						}
					}
					cout<<"repeating edges before: "<<num<<endl;
				}

				//construct replacement for g_working that is actually sparse
				{
					//do 2 loop-overs. the first time around, just find nodes with too many
					//outgoing edges, and merge them in g. 
					//Then in the second iteration, add the first edge whenever there are too many (and do so from the root in g to the root in g)
					for (Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() >= 2){
								//found example of vertex with too many edges
								
								//find id of first element in list
								int to_merge = g->getVertex(g_working->vertices[edge.second.front()]->x, 0, "")->id; 
								
								auto it = edge.second.begin();
								it++; //start at 2nd value of list
								while(it!=edge.second.end()){
									//find all others in g and make sure they're added to the same scc in g
									int other_to_merge = g->getVertex(g_working->vertices[*it]->x, 0, "")->id;
									if(g->dsu.root(other_to_merge) != g->dsu.root(to_merge)){
										//only merge if new information
										g->dsu.merge(
											g->dsu.root(to_merge),
											g->dsu.root(other_to_merge) //calling dsu.root() respects precondition on merge()
										);
									}

									it++;
								}
							}
						}
					}
					
					graph* g_working_2 = new graph;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() >= 2){
								//just add the first edge to g_working_2
								int g_working_v_x = g_working->vertices[edge.second.front()]->x;
								auto v_in_g = g->getVertex(g_working_v_x, 0, "")->id;
								auto root_of_v_in_g = g->dsu.root(v_in_g);
								auto x_of_root_of_v_in_g = g->vertices[root_of_v_in_g]->x;
					
								g_working_2->addEdge(
									//awkward edge flip here; add them in opposite order
									x_of_root_of_v_in_g, 0,
									g->vertices[g->dsu.root(g->getVertex(u->x, 0, "")->id)]->x, 0,
									edge.first.field_name
								);
							}
						}
					}
					g_working_2->dsu.init(g_working_2->N);
					g_working_2->initWorklist();

					//overwrite g_working with g_working_2
					g_working->deleteVertices();
					delete g_working;

					g_working = g_working_2;
				}

				//For now, just print if it is sparse
				if(true){
					int num = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							if(edge.second.size() > 1){
								num++;
								//cout<<"Found example "<<u->x<<" with #edges of label "<<edge.first.field_name<<" equal to "<<edge.second.size()<<endl;
							}
						}
					}
					cout<<"repeating edges after: "<<num<<endl;
				}
				cout<<"size of g after merging for sparseness: "<<g_working->N<<endl;

				{
					int vertices = 0;
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							vertices += edge.second.size();
						}
					}
					cout<<"total vertices in graph: "<<vertices<<endl;
				}

				
				//TODO output if g_working contains more than 1 scc wrt plain reachability
				{
					DSU dsu;
					dsu.init(g_working->N);
					for(Vertex* u : g_working->vertices){
						for(auto edge : u->edges){
							for (auto v_id : edge.second){
								if(dsu.root(u->id) != g->dsu.root(v_id)){
									dsu.merge(dsu.root(u->id), dsu.root(v_id));
								}
							}
						}
					}
					map<int,set<int>> scc;
					for(int i=0;i<g_working->N;i++){
						scc[dsu.root(i)].insert(i);
					}
					cout<<"Found "<<scc.size()<<" disjoint components w.r.t. plain reachability"<<endl;
					int max = -1;
					for(auto el : scc){
						int alt = el.second.size();
						if(alt > max){
							max = alt;
						}
					}
					cout<<"Size of biggest such component: "<<max<<endl;
					
				}


				int reachable_pairs_after_first_reduction = 0;
				{
					graph h = g_working->flatten("[", height); 

					h.bidirectedReach();

					h.forceRootsToLayer(0);

					stillConnectingToNewLayer = g->mergeNodesBasedOnSCCsInFlattened(h, height);

					sccs_after_first_reduction = g->computeSCCs().size();

					reachable_pairs_after_first_reduction = g->calcNumReachablePairs();
					
					if(!stillConnectingToNewLayer){
						cout<<"We're disjoint, so we can stop now"<<endl;
					}


					h.deleteVertices();
				}

				/////

				{
					graph h = g_working->flatten("(", height);

					h.bidirectedReach();

					h.forceRootsToLayer(0);

					stillConnectingToNewLayer = g->mergeNodesBasedOnSCCsInFlattened(h, height);
					
					if(!stillConnectingToNewLayer){
						cout<<"We're disjoint, so we can stop now"<<endl;
					}


					h.deleteVertices();
				}

				g_working->deleteVertices();
				delete g_working; //TODO we're actually supposed to keep working on g_working from here

				//counts how many d1 dot d1 sccs have a +1 self loop on the first counter
				//same for the second counter

				//"for every vertex in g, does it have an edge labeled [ or ( to a node in the same DSCC?"
				int bracket_self_loops = 0;
				int parenthesis_self_loops = 0;
				for(Vertex* v : g->vertices){
					int root_id = g->dsu.root(v->id);
					for (auto edge : v->edges){
						for(auto vid : edge.second){
							if(root_id == g->dsu.root(vid)){
								//edge between two nodes in same DSCC
								if(edge.first.field_name == "["){
									bracket_self_loops = bracket_self_loops + 1;
								}else if(edge.first.field_name == "("){
									parenthesis_self_loops = parenthesis_self_loops + 1;
								}
							}
						}
					}
				}



				t = clock() - t;

				cout<<"Number of D1 dot D1 SCCs before reduction: "<<sccs_before_iteration<<endl;
				cout<<endl;
				cout<<"Number of D1 dot D1 SCCs after first reducing on '[': "<<sccs_after_first_reduction<<endl;
				cout<<"Number of reachable pairs after first reduction on '[': "<<reachable_pairs_after_first_reduction<<endl;
				cout<<endl;
				cout<<"Number of D1 dot D1 SCCs after also reducing on '(': "<<g->computeSCCs().size()<<endl;
				cout<<"Number of reachable pairs after also reducing on '(': "<<g->calcNumReachablePairs()<<endl;
				cout<<endl;
				
				
				cout<<"Number of D1 dot D1 SCCs thus far: "<<g->computeSCCs().size()<<endl;
				cout<<"Number of reachable pairs of g in D1 dot D1: "<<g->calcNumReachablePairs()<<endl;
				cout<<endl;
				cout<<"Number of reachable pairs of g in D ignoring '[': "<<d1<<endl;
				cout<<"Number of reachable pairs of g in D ignoring '(': "<<d2<<endl;
				cout<<"Number of reachable pairs reachable in both of these: "<<reachable_pairs_in_intersection<<endl;
				cout<<endl;
				cout<<"Number of D1 dot D1 SCCs with a +1 self loop on [: "<<bracket_self_loops<<endl;
				cout<<"Number of D1 dot D1 SCCs with a +1 self loop on (: "<<parenthesis_self_loops<<endl;
				cout<<endl;
				cout<<"Time for iteration: "<<(((float)t)/CLOCKS_PER_SEC)<<" s"<<endl;
				cout<<endl;



			}
			delete g;
		}

		return 0;
	}

	if(true){
		//tests early stopping by 'repeating scc patterns'

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
					return 0;
				}
			}


			graph h2 = g.flatten(flatten_on, high_rep + 1);

			h2.bidirectedReach();

			int a = h.calcNumReachablePairs();
			int b = h2.calcNumReachablePairs();
			cout<<"number of pairs when flattening up to "<<height<<": "<<a<<endl;
			cout<<"number of pairs when flattening up to "<<(high_rep + 1)<<": "<<b<<endl;
			if(a != b){
				return 0;
			}

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
			
			g.constructFromDot(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
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
			
			g.constructFromDot(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
			//g.flattenReachRemade(flatten_on);
			
			int height = (int)(((long long) 1000*1000*5) / ((long long)g.N));

			cout<<"flattening graph "<<s<<" up to height "<<height<<endl;

			graph h = g.flatten(flatten_on, height);

			h.bidirectedReach();

			cout<<"D1 dot D1 reachability for "<<s<<"_reduced (when flattening on '"<<flatten_on<<"' up to height "<<height<<"): "<<h.calcNumReachablePairs()<<endl;
		}
		return 0;
	}
}