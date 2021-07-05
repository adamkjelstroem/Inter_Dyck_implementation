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


	if(true){
		//Procedure as of 28 june 2021 2
		string benchmarks[] = {
			//"antlr",
			//"bloat",
			//"chart",
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
				g->construct2(s2, true, true);
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
			cout<<"Computing on "<<s;
			if (using_reduced) cout<<"_reduced";
			cout<<endl;
			g->printSparsenessFacts();
			
			//Compute bidirected reach, as it is a sound under-approximation
			g->bidirectedReach();
			cout<<"Number of reachable pairs via D' reachability: "<<g->calcNumReachablePairs()<<endl;


			
			/*
			for(string l : {"[", "("}){
				graph g_ign_1 = g->copy_ignoring(l);	
				g_ign_1.initWorklist();

				g_ign_1.bidirectedReach();

				cout<<"Number of reachable pairs when ignoring '"<<l<<"':   "<<g_ign_1.calcNumReachablePairs()<<endl;
			}*/


			//use g to construct g_working, which is a copy without duplicate edges
			graph g_working = g->makeCopyWithoutDuplicates();


			//flatten on either label to 2, collapse nodes into 
			/*for(string label : {"(","["}){
				graph h = g->flatten("(", 2);

				h.initWorklist();
				h.bidirectedReach();
				//find sccs and add information to g
				for (Vertex* v : h.vertices){
					if(v->y != 0) continue;
					auto v_root_x = h.vertices[h.dsu.root(v->id)]->x;
					auto v_in_g = g->getVertex(v->x, 0, "")->id;
					auto v_root_in_g = g->getVertex(v_root_x, 0, "")->id;
					auto r1 = g->dsu.root(v_in_g);
					auto r2 = g->dsu.root(v_root_in_g);
					if(r1 != r2){
						g->dsu.merge(r1, r2);
					}
				}
				h.deleteVertices();
			}*/

			cout<<"G_working: "<<endl;
			g_working.printSparsenessFacts();
			if(false){
				graph g_2;
				for(Vertex* v : g_working.vertices){
					for(auto edge : v->edges){
						for(int u_id : edge.second){
							int u_x = g_working.vertices[u_id]->x;
							g_2.addEdge(v->x, 0, u_x, 0, edge.first.field_name);
							g_2.addEdge(u_x, 0, v->x, 0, edge.first.field_name);
						}
					}
				}
				//every vertex in g_2 has an ingoing edge of label 'l' iff the corresponding vertex
				//in g_working has an in or an out edge of label 'l'.
				
				//count the number of these
				int num = 0;
				for(Vertex* v : g_2.vertices){
					if(v->edges.size() == 2){ //It has to be 2 here, as all graphs have an 'eps' edge to itself
						num++; //TODO lookinto this.
					}
				}
				cout<<"Number of vertices that can be removed: "<<num<<endl;


				g_2.deleteVertices();
			}

			if(false){
				auto edge = g_working.vertices[43]->edges;
				cout<<"x of this "<<g_working.vertices[43]->x<<endl;
				cout<<"edges: "<<edge.size()<<endl;
				for(auto e : edge){
					for(auto u : e.second){
					cout<<e.first.field_name<<" with "<<u<<endl;
					}
				}
			}

			{
				graph m = g_working.trim(g_working);
				g_working = m;	
			}
			
			//TODO guarantee that repeating edges is zero

			if(false){	
				cout<<"Since repeating edges is zero, we can start deleting singleton vertices"<<endl;
				graph g_ign_1 = g_working.copy_ignoring("[");	
				graph g_ign_2 = g_working.copy_ignoring("(");

				g_ign_1.initWorklist();
				g_ign_2.initWorklist();

				g_ign_1.bidirectedReach();
				g_ign_2.bidirectedReach();

				//d1 = g_ign_1.calcNumReachablePairs();
				//d2 = g_ign_2.calcNumReachablePairs();


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

				cout<<"Found "<<singletons.size()<<" singletons"<<endl;

				g_ign_1.deleteVertices();
				g_ign_2.deleteVertices();

				graph* g_working_2 = new graph;

				//build working copy of g without the deleted singles edges
				for (Vertex* u : g_working.vertices){
					if(singletons.find(u->id) == singletons.end()){
						//u is not a singleton
						for (auto edge : u->edges){
							for(auto v_id : edge.second){
								if(singletons.find(v_id) == singletons.end()){
									//if v is not a singleton, either
									//then add an edge between them

									//(actually add it between their respective roots)
									auto v_root_w_2 = g_working_2->getVertex(u->x, 0, "");
									auto u_root_w_2 = g_working_2->getVertex(g_working.vertices[v_id]->x, 0, "");

									v_root_w_2->addedge(g_working_2->getfield(edge.first.field_name), u_root_w_2->id);

									g_working.numedges++;
								}
							}
						}
					}
				}


				g_working_2->dsu.init(g_working_2->N);
				g_working_2->initWorklist();

				//overwrite g_working with g_working_2
				g_working.deleteVertices();

				g_working = *g_working_2;
			}

			
			g_working.printSparsenessFacts();

			//output if g_working contains more than 1 scc wrt plain reachability
			int max = 0;
			int root_of_max = -1;
			{
				map<int,set<int>> scc = g_working.computeDisjointSets();
				cout<<"Found "<<scc.size()<<" disjoint components w.r.t plain reachability"<<endl;
				int reasonable = -1;
				for(auto el : scc){
					int alt = el.second.size();
					if(alt > max){
						max = alt;
						root_of_max = el.first;
					}
					

					if(false){
						if(alt < 51 && alt > 20){
							cout<<"digraph example {"<<endl;
							for(int v_id : el.second){
								Vertex* v = g_working.vertices[v_id];
								for(auto edge : v->edges){
									for(auto u_id : edge.second){
										if(u_id != v_id)
											cout<<"	"<<u_id<<" -> "<<v_id<<"[label = \"+1\" color="<<(edge.first.field_name == "[" ? "blue" : (edge.first.field_name == "(" ? "red" : "black"))<<"];"<<endl;
										
									}
								}
							}
							cout<<"}"<<endl;

							return 0;
						}
					}
				}
				cout<<"Size of biggest such component: "<<max<<endl;	
				{
					graph g_part;
					for (auto id_in_g_working : scc[root_of_max]){
						Vertex* v = g_working.vertices[id_in_g_working];
						for(auto edge : v->edges){
							for(auto u_id : edge.second){
								g_part.addEdge(
									g_working.vertices[u_id]->x, 0,
									v->x, 0,
									edge.first.field_name
								);
							}
						}
					}
					g_part.dsu.init(g_part.N);
					g_part.initWorklist();

					//TODO do more trimming following this recipe:

					g_part = g_part.trim(g_part);


					//TODO move this trimming s.t. the entire graph gets trimmed, not just this disjoint subgraph.
					g_part.printSparsenessFacts();
					//g_part.printAsDot();
					return 0;
				}
				if(false){
					cout<<"digraph example {"<<endl;
					for(int v_id : scc[root_of_max]){
						Vertex* v = g_working.vertices[v_id];
						for(auto edge : v->edges){
							for(auto u_id : edge.second){
								if(u_id != v_id)
									cout<<"	"<<u_id<<" -> "<<v_id<<"[label = \"+1\" color="<<(edge.first.field_name == "[" ? "blue" : (edge.first.field_name == "(" ? "red" : "black"))<<"];"<<endl;
								
							}
						}
					}
					cout<<"}"<<endl;
					return 0;
				}

				//extract each component into separate graph
				for(auto s : scc){
					//we don't care about singleton sccs as no information can be discovered
					if(s.second.size() == 1) continue;

					cout<<"Analyzing component of size "<<s.second.size()<<endl;
					
					graph g_part;
					for (auto id_in_g_working : s.second){
						Vertex* v = g_working.vertices[id_in_g_working];
						for(auto edge : v->edges){
							for(auto u_id : edge.second){
								g_part.addEdge(
									g_working.vertices[u_id]->x, 0,
									v->x, 0,
									edge.first.field_name
								);
							}
						}
					}
					g_part.dsu.init(g_part.N);
					g_part.initWorklist();

					if(false){
						cout<<"Number of sccs: "<<g_part.computeSCCs().size()<<endl;
						g_part.bidirectedReach();
						cout<<"Number of sccs after reduction: "<<g_part.computeSCCs().size()<<endl;
					}

					cout<<"This should flatten to a total of "<<g_part.bound() * g_part.N<<" nodes."<<endl;

					//compute flatten, then reach on each of these
				
					//graph h_part = g_part.flatten("(", g_part.bound());
					graph h_part = g_part.flatten("(", 100);
					
					g_part.deleteVertices();

					h_part.bidirectedReach();	
					h_part.forceRootsToLayer(0);

					//then merge info into g
					h_part.transplantReachabilityInformationTo(*g);

					h_part.deleteVertices();
				}
				
			}


			g_working.deleteVertices();

			cout<<"Number of reachable pairs in g: "<<g->calcNumReachablePairs()<<endl;

			g->deleteVertices();
			delete g;
		}

		return 0;
	}




	if(true){
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
				g->construct2(s2, true, true);
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
				g->construct2(s2, true, true);
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
					cout<<"Found "<<scc.size()<<" disjoint components w.r.t plain reachability"<<endl;
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

	//tests flattenReach w/ memory
	if(false){
		//TODO broken??
		graph g;
		string s = 
			"antlr";
		string s2 = "./spg/reduced_bench/" + s + "_reduced.dot";

		g.construct2(s2, true, true);

		g.flattenReach("[");

		
		cout<<"d1 dot d1 reachability for reduced version of "<<s<<": "<<g.calcNumReachablePairs()<<endl;

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
			g.construct2(s2, true, true); //Parses files in the ".dot" format as D1 dot D1
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


			//clock_t time;
			struct timeval tv1,tv2;


			gettimeofday(&tv1, NULL);
			//time = clock();

			g.heuristicReductionBeforeFlattenReach(flatten_on);


			//time = clock() - time;
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