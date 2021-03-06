#include "graph/graph.h"
#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>


//construct graph using the ".dot" format. 
//d1_parenthesis flag relaxes on parentheses, and d1_bracket relaxes on brackets.
void graph::constructFromDot(string infile_name, bool d1_parenthesis, bool d1_bracket){
	ifstream infile(infile_name);
	string line;
	while(std::getline(infile, line)){
		// if this is a non-comment line
		if(line.find("//") != string::npos){
			continue; //skip commented lines
		}
		if(line.find("->") != string::npos){
			std::vector<string> tokens;
			split(line, "->", tokens);
			int a = std::stoi(tokens[0]);
			std::vector<string> tokens2;
			split(tokens[1], "[label=\"", tokens2);
			int b = std::stoi(tokens2[0]);
			string label = tokens2[1];

			std::vector<string> tokens3;
			split(label, "-", tokens3);

			string id = tokens3[1];
			
			std::vector<string> tokens4;
			split(id, "\"", tokens4);
			id = tokens4[0];

			//ignores ids to relax on parenthesis
			if (d1_parenthesis && label.find("p") != string::npos)id = "";
			//ignores ids to relax on brackets
			if (d1_bracket && label.find("b") != string::npos) id = "";

			//parse
			//note the 'flipping' s.t. we "add an edge" from a to b by calling in order (b, a, label)
			if (label.find("op") != string::npos){
				addEdge(b, 0, a, 0, "(" + id);
			}else if (label.find("cp") != string::npos){
				addEdge(a, 0, b, 0, "(" + id);
			}else if (label.find("ob") != string::npos){
				addEdge(b, 0, a, 0, "[" + id);
			}else if (label.find("cb") != string::npos){
				addEdge(a, 0, b, 0, "[" + id);
			}else{
				addEdge(a, 0, b, 0, EPS.field_name);
			}	
			//cout<<"adding edge from "<<a<<" to "<<b<<" with label= "<<label<<endl;
		}
	}
	dsu.init(vertices.size());
}

//performs graph flattening on 'field_name' to depth 'depth'
graph graph::flatten(string field_name, int depth){
	graph g;
	auto cop = [](Vertex a, Vertex b, field f, void* extra[]) {
			graph* g = (graph*)extra[0];
			int i = *((int*)extra[1]);
			int depth = *(int*)extra[2];
			string field_name = *(string*)extra[3];
			if(f.field_name == field_name){ //TODO cache id of "[" field and do comparison on
				//flatten on this field
				if(i+1!=depth)
					g->addEdge(
						a.x, i, 
						b.x, i+1, 
						g->EPS.field_name
					);
			}else{
				g->addEdge(
					a.x, i,
					b.x, i,
					f.field_name
				);
					
			}
		};
	//cout<<"Flattening on "<<field_name<<" up to height "<<depth<<endl;
	

	clock_t t, t2;
	t = clock();
	bool print = false;
	
	for (int i = 0; i < depth; i++){
		if(print && clock() - t > 3*CLOCKS_PER_SEC){
			cout<<"doing iteration "<<i<<" of "<<depth<<" (t="<<((float)clock()-t)/CLOCKS_PER_SEC<<") "<<(i * 100 / depth)<<"%"<<endl;
			t = clock();
		}
		
		//if(depth >= 10 && i % (depth / 10) == 0) cout<<"doing iteration "<<i<<" of "<<depth<<endl;
		
		void* w[] = {&g, &i, &depth, &field_name};
		iterateOverEdges(cop, w);
	}

	g.dsu.init(g.N);
	g.initWorklist();

	return g;
}

//returns the vertex in g that corresponds to 'me'.
Vertex* getVertexIn(graph& g, Vertex* me){
	return g.getVertex(me->x, 0, "");
}

void graph::transplantReachabilityInformationTo(graph& g){
	forceRootsToLayer(0);
	auto sccs = computeSCCs();

	int mergings = 0;
	for(auto s : sccs){
		auto root = vertices[s.first];

		//we only care about roots at layer 0
		if(root->y != 0) continue;
		
		for(auto b : s.second){
			auto other = vertices[b];

			//we only care about members of the scc which are also at layer 0
			if(other->y != 0) continue;
			
			auto root_in_g_s_root_id = g.dsu.root(getVertexIn(g, root)->id);
			auto other_in_g_s_root_id = g.dsu.root(getVertexIn(g, other)->id);

			if(root_in_g_s_root_id != other_in_g_s_root_id){
				g.dsu.merge(root_in_g_s_root_id, other_in_g_s_root_id);
				mergings++;
			}
		}
	}

	// if(mergings > 0) cout<<"merged "<<(mergings+1)<<" nodes in g"<<endl;
}


//Implementation of the D1 odot D1 algorithm.
//PRECONDITION: this method is called on a graph of D1D1 form.
void graph::bidirectedInterleavedD1D1Reach(){
	//Reduce graph via bidirected reach, as it is a sound under-approximation
	bidirectedReach();

	//construct working copy of g without dublicate edges.
	graph g_working = makeCopyWithoutDuplicates();
	
	//use various rules of trimming to reduce graph. 
	//Precondition: bidirectedReach(), then makeCopyWhoutoutDublicates() have been called
	g_working = g_working.trim_d1d1(g_working);

	//split graph into disjoint components
	map<int,set<int>> disjoint_components = g_working.computeDisjointSets();
	
	//further split each component if possible, then flatten up to the bound and perform
	//calculations directly.
	for(auto s : disjoint_components){
		graph g_part = g_working.buildSubgraph(s.second);
		g_part.removeHubVertexIfExistsThenCalc(g_part, *this);
	}
	
	//cleanup
	g_working.deleteVertices();
}

//Implementation of the Dk odot D1 algorithm. 
//PRECONDITION: this method is called on a graph of DkD1 form.
void graph::bidirectedInterleavedDkD1Reach(string flatten_on){
	//Reduce graph via bidirected reach, as it is a sound under-approximation
	bidirectedReach();
	
 	//construct working copy of g without dublicate edges.
	graph g_working = makeCopyWithoutDuplicates();

	//use various rules of trimming (which apply in the DkD1 case) to reduce graph. 
	//Precondition: bidirectedReach(), then makeCopyWhoutoutDublicates() have been called
	g_working = g_working.trim_dkd1(g_working);
	g_working.initWorklist();

	//split graph into disjoint components and compute on each directly.
	auto disjoint_components = g_working.computeDisjointSets();
	for(auto component : disjoint_components){
		graph subgraph = g_working.buildSubgraph(component.second);

		//subgraph.N is the bound
		graph h = subgraph.flatten(flatten_on, subgraph.N);

		subgraph.deleteVertices();
				
		h.bidirectedReach();
		
		h.transplantReachabilityInformationTo(*this);

		h.deleteVertices();
	}
	//cleanup
	g_working.deleteVertices();
}

map<int,set<int>> graph::computeDisjointSets(){
	DSU dsu;
	dsu.init(N);
	for(Vertex* u : vertices){
		for(auto edge : u->edges){
			for (auto v_id : edge.second){
				if(dsu.root(u->id) != dsu.root(v_id)){
					dsu.merge(dsu.root(u->id), dsu.root(v_id));
				}
			}
		}
	}
	map<int,set<int>> scc;
	for(int i=0;i<N;i++){
		scc[dsu.root(i)].insert(i);
	}
	return scc;
}

map<int,set<int>> graph::computeSCCs(){
	map<int,set<int>> scc;
	for(int i=0;i<N;i++){
		scc[dsu.root(i)].insert(i);
	}
	return scc;
}

//forces roots of the sets in the DSU to be in layer 'layer'
void graph::forceRootsToLayer(int layer){
	for(int i = 0; i < N; i++){
		if(vertices[i]->y == layer){
			if(vertices[dsu.root(i)]->y != layer){
				dsu.exchange(i, dsu.root(i));
			}
		}
	}
}

//returns true iff top layer of h and bottom layer of h contract.
//height denotes height of top layer of h
bool graph::mergeNodesBasedOnSCCsInFlattened(graph h, int height){
	map<int,set<int>> scc = h.computeSCCs();
	bool stillCOnnectingToNewLayer = false;

	//merge vertices in original graph
	auto it = scc.begin();
	while(it!=scc.end()){
		//we only care about sccs where the root has y=0
		if(h.vertices[it->first]->y == 0){
			int root_id_in_this=dsu.root(getVertex(h.vertices[it->first]->x, 0, "")->id);
			for(int elem : it->second){
				if(h.vertices[elem]->y == 0){
					dsu.merge(
						root_id_in_this,
						dsu.root(getVertex(h.vertices[elem]->x, 0, "")->id)
						);
				}else if(h.vertices[elem]->y == height-1){
					stillCOnnectingToNewLayer = true;
				}
			}
		}
		it++;
	}
	return stillCOnnectingToNewLayer;
}

void graph::iterateOverEdges(void (f)(Vertex start, Vertex end, field f, void* extra[]), void* extra[]){
	int j_root;
	for(int j=0;j<N;j++){
		auto vertex = vertices[j];
		auto fit = vertex->edgesbegin();
		j_root = dsu.root(j); //optimization so calls to dsu.root() won't happen every loop
		while(fit!=vertex->edgesend()){   // iterating over field
			field fi = fit->first;
			auto fedgeit = vertex->edgesbegin(fi);
			while(fedgeit != vertex->edgesend(fi)){   // iterating over edges
				//(*f)(*vertices[*fedgeit], *vertices[j], fi, extra);
				(*f)(*vertices[dsu.root(*fedgeit)], *vertices[j_root], fi, extra);

				fedgeit++;
			}
			fit++;
		}
	}
}

//Produces a copy of this. Note that IDs are not preserved, but x and y values are.
graph graph::copy(){
	graph g;

	auto cop = [](Vertex a, Vertex b, field f, void* extra[]) {
		graph* g = (graph*)extra[0];
		g->addEdge(a.x, a.y, b.x, b.y, f.field_name);
	};
	void* w[] = {&g};
	iterateOverEdges(cop, w);

	g.dsu.init(g.N);

	return g;
}

//copies the graph, replacing edges with 'label' with 'eps'. Note that IDs are not preserved, but x and y values are.
graph graph::copy_ignoring(string label){
	graph g;

	auto cop = [](Vertex a, Vertex b, field f, void* extra[]) {
		graph* g = (graph*)extra[0];
		string label = *(string*)extra[1];
		if(f.field_name == label){
			g->addEdge(a.x, a.y, b.x, b.y, "eps");
		}else{
			g->addEdge(a.x, a.y, b.x, b.y, f.field_name);
		}
	};
	void* w[] = {&g, &label};
	iterateOverEdges(cop, w);

	g.dsu.init(g.N);

	return g;
}


//makes copy without duplicates, adding edges between roots.
graph graph::makeCopyWithoutDuplicates(){
	graph g_working;
	for(Vertex* v : vertices){
		auto v_root = vertices[dsu.root(v->id)]; //find v's root vertex in g
		auto v_root_w = getVertexIn(g_working, v_root); //find the vertex with the corresponding x value in g_working
		for(auto edge : v->edges){
			for(int u_id : edge.second){
				int u_root_id = dsu.root(u_id);
				auto u_root_w = getVertexIn(g_working, vertices[u_root_id]);
				
				auto e = v_root_w->edges[edge.first];

				if (std::find(e.begin(), e.end(), u_root_w->id) == e.end()){
					//we do not have an edge yet
					v_root_w->addedge(g_working.getfield(edge.first.field_name), u_root_w->id);

					g_working.numedges++;
				}
			}
		}
	}

	g_working.dsu.init(g_working.N);
	g_working.initWorklist();
	return g_working;
}

int graph::calcNumReachablePairs(){
	int n = 0;

	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}

	auto it = scc.begin();
	while(it!=scc.end()){
		int zero_elems = 0;
		for(int elem : it->second){
			if(vertices[elem]->y == 0) zero_elems++;
		}
		n += zero_elems*(zero_elems - 1) / 2;
		it++;
	}

	return n;
}

void graph::printGraphAsTikz(){
	cout<<"\\begin{tikzpicture}"<<endl;

	int d = 1;

	cout<<endl;
	if(isFlattened){
		//graph is flattened

		for (Vertex* v : vertices){
			cout<<"\\node ("<<v->id<<") at ("<<v->x * d<<", "<<v->y*d<<") {("<<v->x<<","<<v->y<<")"<<""<<"};"<<endl;
		}

		auto print = [](Vertex a, Vertex b, field f, void* extra[]) {
			if(a.id != b.id){
				if (f.field_name == "eps"){
					cout<< "\\path ("<<b.id<<") edge node {$ $} ("<<a.id<<");"<<endl;
				}else{
					cout<< "\\path [->, red] ("<<b.id<<") edge [bend right=45] node {$ $} ("<<a.id<<");"<<endl;
				}
			}
		};
		iterateOverEdges(print, nullptr);
	}else{
		//graph is not flattened

		for (Vertex* v : vertices){
			cout<<"\\node ("<<v->id<<") at ("<<v->x*d<<", 0) {"<<v->x<<"};"<<endl;
		}

		auto print = [](Vertex a, Vertex b, field f, void* extra[]) {
			if(a.id != b.id){
				if (f.field_name == "["){
					cout<< "\\path [->, blue] ("<<b.id<<") edge [bend left=20] node {$ $} ("<<a.id<<");"<<endl;
				}else if(f.field_name == "("){
					cout<< "\\path [->, red] ("<<b.id<<") edge [bend right=20] node {$ $} ("<<a.id<<");"<<endl;
				}else{
					cout<< "\\path ("<<b.id<<") edge [bend right=10] node {$ $} ("<<a.id<<");"<<endl;
				}
			}
		};

		iterateOverEdges(print, nullptr);
	}

	cout<<"\\end{tikzpicture}\\\\"<<endl;

}

//prints the graph in the 'dot' format
void graph::printAsDot(){
	cout<<"digraph example {"<<endl;
	for(Vertex* v : vertices){
		for(auto edge : v->edges){
			for(auto u_id : edge.second){
				if(!(u_id == v->id && edge.first.field_name == "eps")){
					cout<<"	"<<vertices[u_id]->id<<" -> "<<v->id;
					cout<<"[label = \""<<edge.first.field_name<<"\" color=";
					cout<<(edge.first.field_name.find("[") != std::string::npos ? "blue" : (edge.first.field_name.find("(") != std::string::npos ? "red" : "black"))<<"];"<<endl;

				}
				
			}
		}
	}
	cout<<"}"<<endl;
}

void graph::printAsDot2(){
	for(Vertex* v : vertices){
		for(auto edge : v->edges){
			for(auto u_id : edge.second){
				if(!(u_id == v->id && edge.first.field_name == "eps")){
					cout<<vertices[u_id]->id<<" -> "<<v->id;
					cout<<"[label = \""<<edge.first.field_name<<"\"];"<<endl;
				}
			}
		}
	}
}




//counts 'true' self loops, meaning we ignore the mandatory eps edge
int countSelfLoops(Vertex* v){
	int count = 0;
	for(auto edge : v->edges){
		for(int id : edge.second){
			if(id == v->id && edge.first.field_name != "eps") count++;
		}
	}
	return count;
}

//counts 'true' in-edges, ignoring self-loops
int countInEdges(Vertex* v){
	int count = 0;
	for(auto edge : v->edges){
		for(int id : edge.second){
			if(id != v->id) count++;
		}
	}
	return count;
}

//counts 'true' out-edges, ignoring self-loops
int countOutEdges(Vertex* v, graph& g_flipped){
	return countInEdges(getVertexIn(g_flipped, v));
}

//builds flipped version of g, meaning edges go in the other direction.
graph buildFlipped(graph &g){
	graph g_outgoing; //g_outgoing is g_working but with edges flipped
	for(Vertex* v : g.vertices){
		for(auto edge : v->edges){
			for(int u_id : edge.second){
				int u_x = g.vertices[u_id]->x;
				g_outgoing.addEdge(v->x, 0, u_x, 0, edge.first.field_name);
			}
		}
	}
	return g_outgoing;
}

void printVertexInfo(Vertex* v, graph& g_flipped){
	cout<<"looking at x = "<<v->x<<" with id "<<v->id<<endl;
	cout<<"self-loops: "<<countSelfLoops(v)<<endl;
	cout<<"out-edges: "<<countOutEdges(v, g_flipped)<<endl;
	cout<<"in-edges: "<<countInEdges(v)<<endl;
}

//if b has one in-edge, and b has no out-edges, and b has no self-loops,
//then, remove b
void findRemovableVerticesViaFirstRule(graph &g_working, graph &g_flipped, set<int> &to_delete){
	for(Vertex* b : g_working.vertices){
		if(countInEdges(b) != 1) continue;
		if(countOutEdges(b, g_flipped) != 0) continue;
		if(countSelfLoops(b) != 0) continue;
		to_delete.insert(b->id);
	}	
}

/*	
if b does not have outgoing edges to other nodes than itself;

and b only has one neighbor

AND either

case a):
- node b has exactly one incoming edge, and b's self-loop is also a self-loop of its neighbor

OR

case b):
- you have 2 incoming edges, and both are self-loops on your neighbor

then, we can safely remove b
*/
void findRemovableVerticesViaSecondRule(graph &g_working, graph &g_flipped, set<int> &to_delete){

	for(Vertex* b : g_working.vertices){
		//if b has only one neighbor, aka all nodes that 
		//reach b are itself or one specific node:
		auto b_outgoing = getVertexIn(g_flipped, b);
		bool failed = false;

		//check if b has outgoing edges only to itself
		string self_loop_label = "";
		for(auto edge : b_outgoing->edges){
			for(auto neighbor : edge.second){
				if(neighbor != b_outgoing->id){
					failed = true;
				}else if (edge.first.field_name != "eps"){
					self_loop_label = edge.first.field_name;
				}
			}
		}
		if(failed) continue;

		//check if b only has incoming edges from itself or one specific other node
		int neighbor_id = -1;
		int edges_from_neighbor = 0;
		for(auto edge : b->edges){
			for(auto neighbor : edge.second){
				if(neighbor != b->id){
					if(neighbor_id == -1){
						neighbor_id = neighbor;
						edges_from_neighbor++;
					}else if (neighbor != neighbor_id){
						failed = true;
					}else{
						edges_from_neighbor++;
					}
				}
			}
		}
		if(failed) continue;

		if(edges_from_neighbor == 1){
			//case a):
			//- node b has exactly one incoming edge, and b's self-loop is also a self-loop of its neighbor
			bool yes = false;
			for(auto edge : g_working.vertices[neighbor_id]->edges){
				for(auto other : edge.second){
					if(other == neighbor_id){
						//found a self loop on the neighbor
						if(edge.first.field_name == self_loop_label){
							yes = true;
						}
					}
				}
			}
			if(yes){
				to_delete.insert(b->id);
			}
		}else if(edges_from_neighbor == 2){
			//case b):
			//- b has 2 incoming edges, and both are self-loops on its neighbor
			int self_loops_on_neigbor = 0;
			for(auto edge : g_working.vertices[neighbor_id]->edges){
				for(auto other : edge.second){
					if(other == neighbor_id)
						self_loops_on_neigbor++;
				}
			}
			if(self_loops_on_neigbor == 3){ //all nodes have an eps self-edge, so this is how to ask for 2 'true' self-loops
				//b has 2 incoming edges, and its neighbor has 2 self-loops. thus we are done
				to_delete.insert(b->id);
			}
		}
	}
}

//if a has no self-loops or in-edges,
//and b has no self-loops or out-edges
//and a has only one out-edge to (b)
//then remove a
void findRemovableVerticesViaThirdRule(graph &g_working, graph &g_flipped, set<int> &to_delete){
	for(Vertex* b : g_working.vertices){
		//printVertexInfo(b, g_flipped);
		if(countSelfLoops(b) != 0) continue;
		if(countOutEdges(b, g_flipped) != 0) continue;

		//find 'a'
		Vertex* a;
		for(auto edge : b->edges){
			for (int id : edge.second){
				if(id == b->id) continue;
				a = g_working.vertices[id];

				//test if 'a' adheres to rules
				if(countSelfLoops(a) != 0) continue;
				if(countInEdges(a) != 0) continue;

				to_delete.insert(a->id);
			}
		}
	}
}

void discoverDeletableVertices(graph &g_working, set<int> &to_delete){
	graph g_flipped = buildFlipped(g_working);

	findRemovableVerticesViaFirstRule(g_working, g_flipped, to_delete);

	findRemovableVerticesViaSecondRule(g_working, g_flipped, to_delete);

	findRemovableVerticesViaThirdRule(g_working, g_flipped, to_delete);

	g_flipped.deleteVertices();
}

//if a --> c <-- b and a --> d <-- b, then delete d.
void findRemovableVerticesWhereAllWitnessSame(graph &g_working, set<int> &to_delete){
	graph g_flipped = buildFlipped(g_working);

	for(Vertex* a : g_working.vertices){
		Vertex* a_flipped = getVertexIn(g_flipped, a);
		for(auto edge : a_flipped->edges){
			if(edge.first.field_name != "[") continue;

			map<int, set<int>> map1;
			
			for(int c_flipped_id : edge.second){
				Vertex* c = getVertexIn(g_working, g_flipped.vertices[c_flipped_id]);
				
				if(countSelfLoops(c) != 0) continue;
				if(countOutEdges(c, g_flipped) != 0) continue;
				if(countInEdges(c) != 2) continue;

				//c has 2 in-edges and 0 loops and 0 out-edges

				Vertex* b;
				for(auto e : c->edges){
					for(int b_id : e.second){
						if(b_id != c->id)
							b = g_working.vertices[b_id];
					}
				}

				map1[b->id].insert(c->id);
			}
			//if(map1.size() > 1) cout<<"found "<<map1.size()<<" examples"<<endl;

			for(auto entry : map1){
				if(entry.second.size() <= 1) continue;

				int saved = -1;
				for(int el : entry.second){
					if(saved == -1){
						saved = el; //save first one discovered
					}else{
						to_delete.insert(el); //mark all others for deletion.
					}
				}
			}
		}
	}


	g_flipped.deleteVertices();
}

graph buildCopyWithout(graph& g_working, set<int>& to_delete){
	graph g_working_2;

	//build working copy of g without the deleted vertices
	//basically, add an edge between 2 nodes if neither is to be deleted.
	for (Vertex* u : g_working.vertices){
		if(to_delete.find(u->id) == to_delete.end()){
			//u is not a singleton
			for (auto edge : u->edges){
				for(auto v_id : edge.second){
					if(to_delete.find(v_id) == to_delete.end()){
						//if v is not a singleton, either
						//then add an edge between them

						//(actually add it between their respective roots)
						auto v_root_w_2 = getVertexIn(g_working_2, u);
						auto u_root_w_2 = getVertexIn(g_working_2, g_working.vertices[v_id]);

						v_root_w_2->addedge(g_working_2.getfield(edge.first.field_name), u_root_w_2->id);

						g_working.numedges++;
					}
				}
			}
		}
	}


	g_working_2.dsu.init(g_working_2.N);
	g_working_2.initWorklist();

	//overwrite g_working with g_working_2
	//g_working.deleteVertices();
	return g_working_2;
}

//Helper function for removeHubVertexIfExistsThenCalc.
map<int, set<int>> getDisjointSetsWhenRemoving(graph& g_working, Vertex* without){
	set<int> u_set;
	u_set.insert(without->id);

	graph g_working_without_u = buildCopyWithout(g_working, u_set);

	auto disjoint_subgraphs_not_in_g_working = g_working_without_u.computeDisjointSets();

	//fix so ids in disjoint_subgraphs are actually ids in g_working
	map<int, set<int>> disjoint_subgraphs;
	for(auto el : disjoint_subgraphs_not_in_g_working){
		for (int id : el.second){
			Vertex* v_in_g_working = getVertexIn(g_working, g_working_without_u.vertices[id]);
			disjoint_subgraphs[el.first].insert(v_in_g_working->id);
		}
		disjoint_subgraphs[el.first].insert(without->id);
	}

	return disjoint_subgraphs;
}

/*
Assume that u reaches v via some path P. Then without loss of generality, this path self-loops on u and then never enters u again. 
Given this observation:

1. Remove u from G
2. Let G_1,G_2,... be the connected components after removing u
3. For each such G_i, insert u back in G_i and see if u reaches any v in G_i
4. If you find that u reaches some v in some G_i, do the appropriate merging to turn the initial graph G to a new graph G'. 
   G' will also have a double self loop on u (after merging), so repeat the above process.
5. If you find that u does not reach any v in any G_i, remove u from G. Call G' the new graph, and repeat.


equivalent procedure as implemented:

search for vertex u s.t. u has self edge on both counters

if u exists:
	while true:
		remove u from G
		find disjoint sets
		for each set:
			insert u into each
			flatten to bound
			compute all-pairs reachability
			update G with this info (shrink it)
		if no new info was discovered:
			return		//since we already computed all-pairs reachability, we don't need to do the same without u for each set.
if u does not exist:
	flatten to bound
	compute all-pairs reachability
	update G with this info

*/
void graph::removeHubVertexIfExistsThenCalc(graph &g_working, graph &g_orig){
	//first, discover u
	Vertex* u;
	bool hasDoubleSelfLoop = false;
	for(Vertex* v : g_working.vertices){
		if(countSelfLoops(v) != 2) continue;
		u = v;
		hasDoubleSelfLoop = true;
		break;
	}

	//the process makes no sense if we don't have such a vertex.
	while(hasDoubleSelfLoop){
		Vertex* u_in_g_working = getVertexIn(g_working, u);
		map<int, set<int>> disjoint_subgraphs = getDisjointSetsWhenRemoving(g_working, u_in_g_working);

		int total_merged_with_u = 0;
		for(auto el : disjoint_subgraphs){
			auto ids_of_subgraph = el.second;

			graph subgraph = g_working.buildSubgraph(ids_of_subgraph);
			
			//Note: this part takes a while if subgraph is big.
			graph h = subgraph.flatten("[", subgraph.bound());
			h.bidirectedReach();
			
			//transplant reachability info to both original and working versions of graph
			h.transplantReachabilityInformationTo(g_orig);
			h.transplantReachabilityInformationTo(g_working);
			
			{
				auto h_sccs = h.computeSCCs();
				Vertex* u_in_h = getVertexIn(h,u_in_g_working);
					
				int root_of_u_in_h = h.dsu.root(u_in_h->id);
				
				auto scc_with_u_in_h = h_sccs[root_of_u_in_h];			
				
				int count = 0;
				for(int member : scc_with_u_in_h){
					if(h.vertices[member]->y == 0){
						count++;
					} 
				}
				total_merged_with_u += count - 1;
			}
			h.deleteVertices();
			subgraph.deleteVertices();
		}

		//cout<<"Total vertices that get joined with 'u': "<<total_merged_with_u<<endl;
		if(total_merged_with_u == 0) return;


		auto graph1 = g_working.makeCopyWithoutDuplicates();
		g_working.deleteVertices();
		g_working = graph1;
	}

	//if we get here, g_working does not have any such hub.
	//thus we just compute bidirected reachability on it directly
	//and merge that info into the original.

	//Note: this part takes a while if g_working is big.
	graph h = g_working.flatten("[", g_working.bound());
	h.bidirectedReach();

	//transplant reachability info to both original and working versions of graph
	h.transplantReachabilityInformationTo(g_orig);
	h.transplantReachabilityInformationTo(g_working);
	h.deleteVertices();
}

//Trims graph in the D1D1 case, removing unreachable nodes that are also irrelevant for reachability between other nodes.
graph graph::trim_d1d1(graph& g_working){
	while(true){
		set<int> to_delete;
		graph g_working_2;

		discoverDeletableVertices(g_working, to_delete);
		if(to_delete.size() == 0) break;
		
		g_working_2 = buildCopyWithout(g_working, to_delete);
		g_working.deleteVertices();
		g_working = g_working_2;

		set<int> to_delete2;
		findRemovableVerticesWhereAllWitnessSame(g_working, to_delete2);
		g_working_2 = buildCopyWithout(g_working, to_delete2);
		g_working.deleteVertices();
		g_working = g_working_2;

		//cout<<"Newly reduced g, without the removable edges:"<<endl;
		//g_working.printSparsenessFacts();
	}

	return g_working;
}

//Trims graph in the DkD1 case, removing unreachable nodes that are also irrelevant for reachability between other nodes.
//This means applying the first and third trimming rules.
graph graph::trim_dkd1(graph& g_working){
	while(true){
		set<int> to_delete;
		graph g_working_2;

		
		graph g_flipped = buildFlipped(g_working);

		findRemovableVerticesViaFirstRule(g_working, g_flipped, to_delete);

		findRemovableVerticesViaThirdRule(g_working, g_flipped, to_delete);

		g_flipped.deleteVertices();
		
		if(to_delete.size() == 0) break;
		
		g_working_2 = buildCopyWithout(g_working, to_delete);
		g_working.deleteVertices();
		g_working = g_working_2;

		//cout<<"Newly reduced g, without the removable edges:"<<endl;
		//g_working.printSparsenessFacts();
	}

	return g_working;
}

graph graph::buildSubgraph(set<int> &ids){
	graph g_part;
	for (auto id_in_g_working : ids){
		Vertex* v = vertices[id_in_g_working];
		
		for(auto edge : v->edges){
			for(int u_id : edge.second){
				//ignore vertices that are not in subgraph
				//if(ids.find(u_id) == ids.end()) continue;

				g_part.addEdge(
					vertices[u_id]->x, 0,
					v->x, 0,
					edge.first.field_name
				);
			}
		}
	}
	g_part.dsu.init(g_part.N);
	g_part.initWorklist();
	return g_part;
}

void graph::initWorklist(){
	//cout<<"Number of vertices : "<<vertices.size()<<endl;
	//cout<<"Number of edges : "<<numedges<<endl;
	//cout<<"Number of field types : "<<fields.size()<<endl;

	for(int i=0;i<vertices.size();i++){//change this
		Vertex* vtx = vertices[i];
		if(i!=dsu.root(i)){
			vtx->clear();
			continue;
		}
		// vtx->clear(EPS);
		auto it = vtx->edgesbegin(); //pointer to pair<field,list<int>>
		while(it!=vtx->edgesend()){
			removeRepeatedges(it->second);
			if(it->second.size()>=2) {
				worklist.push(pair<int,field>(vtx->id,it->first));
			}
			it++;
		}
	}
}

// algorithm for computing bidirected reachability on the union alphabet.
void graph::bidirectedReach(){
	//precondition graph::initWorklist() has been called
	while(!worklist.empty()){
		pair<int,field> p = worklist.front();
		worklist.pop();
		Vertex* vtx =  vertices[p.first];
		if(p.first == dsu.root(p.first) ){
			std::set<int> roots;
			int rootS = -1;
			int rootA = getRoot(vtx,p.second,roots,rootS);
			if(roots.size()<2){
				// can roots have zero size??
				// assert(roots.size()!=0);
				vtx->set(p.second,*(roots.begin()));
				continue;
			}
			dsuUnion(rootA,roots);
			if(rootA==vtx->id){
				// assert(rootS!=-1);
				dsu.exchange(rootA,rootS);
				rootA=rootS;
			}
			Vertex* rootvtx = vertices[rootA];
			appendedges(roots,rootvtx,vtx->id,p.second);
			addnewPair(rootvtx);
			vtx->set(p.second,rootA);
		}
	}
}

void graph::dsuUnion(int rootA,std::set<int>& roots){
	auto it = roots.begin();
	// auto end = roots.end();
	while(it!=roots.end()){
		dsu.hardmerge(rootA,*it);
		it++;
	}
}

// finds all the vertex's root pointed by vtx and field
// returns the merge vtx id of root of the merged disjoint set
int graph::getRoot(Vertex* vtx,field& f,std::set<int>& roots,int& rootS){
	auto it = vtx->edgesbegin(f); //pointer to vertex id (int)
	// auto end = vtx->edgesend(f);
	int rootA = -1; //invariant: rootA = max rank root so far excluding vtx
	int rankA = 0;
	int rankS = 0;
	while(it!=vtx->edgesend(f)){  //merge
		int rootB = dsu.root(*it);
		int rankB = dsu.rank(rootB);
		if(rankB>rankS){
			if(rankB>rankA){
				rootS=rootA; rankS=rankA;
				rootA=rootB; rankA=rankB;
			}else if(rootA!=rootB){
				rootS=rootB; rankS=rankB;
			}
		}
		roots.insert(rootB);
		it++;
	}
	return rootA;
}

// append all the edges in roots to rootvtx
void graph::appendedges(std::set<int>& roots,Vertex *rootvtx, int uid,field mf){
	for(auto i=roots.begin();i!=roots.end();i++){
		Vertex* rvtx = vertices[*i];
		if(rvtx->id == rootvtx->id)
			continue;
		auto it = rvtx->edgesbegin();
		// auto end = rvtx->edgesend();
		while(it!=rvtx->edgesend()){
			field f = it->first;
			if(rvtx->id==uid and f==mf){
				rootvtx->addedge(f,rootvtx->id);
				it++;
				continue;
			}
			rootvtx->append(it->first,it->second);
			// assert(rvtx->edgesNumber(it->first)==0);
			it++;
		}
	}
}

// add <rootvertex,field> to worklist if possible
void graph::addnewPair(Vertex* rootvtx){
	//worklist add. scope for optimisation
	auto it = rootvtx->edgesbegin();
	auto end = rootvtx->edgesend();
	while(it!=end){
		if(it->second.size()>=2)
			worklist.push(pair<int,field>(rootvtx->id,it->first));
		it++;
	}
}


void graph::removeRepeatedges(list<int> &l){
	set<int> elem;
	auto it = l.begin();
	while(it!=l.end()){
		if( elem.find(dsu.root(*it))!=elem.end() ){
			it = l.erase(it);
		}else{
			elem.insert(*it = dsu.root(*it));
			it++;
		}
	}
}

// query returns true iff variable with uid may point to variable with vid
bool graph::query(int uid,int vid){
	return dsu.root(uid)==dsu.root(vid);
}



Vertex* graph::getVertex(int x, int y, const string &name){
	auto it = pos2vtx[x].find(y);
	//auto it = str2vtx.find("");
	if(it==pos2vtx[x].end()){
		Vertex* vtx = new Vertex(this->N, x, y,name);
		//cout<<"adding new edge with id \""<<vtx->id<<"\" and name \""<<vtx->name<<"\""<<endl;
		vertices.push_back(vtx);
		vtx->addedge(EPS,vtx->id);
		this->N++;
		pos2vtx[x][y] = vtx;
		//str2vtx[s + ": " + to_string(layer)]=vtx;
		return vtx;
	}
	return it->second;
}

field& graph::getfield(const string &s){
	auto it = str2field.find(s);
	if(it==str2field.end()){
		field f(fields.size(),s);
		fields.push_back(f);
		str2field[s]=fields[fields.size()-1];
		return fields[fields.size()-1];
	}
	return it->second;
}

//adds edge from start to end with label l. example: l="(" means   a -- "(" --> b
void graph::addEdge(int start_x, int start_y, int end_x, int end_y, string label){
	Vertex* start = getVertex(start_x, start_y, "");
	Vertex* end   = getVertex(end_x,   end_y, "");

	//cout<<"addEdge from "<<start->to_string()<<" to "<<end->to_string()<<"\\\\"<<endl;
	
	end->addedge(getfield(label), start->id);

	setFlattened(end_y != 0 | start_y != 0);
	numedges++;
}


void graph::printSparsenessFacts(){
	/*
	int num = 0;
	for(Vertex* u : vertices){
		for(auto edge : u->edges){
			if(edge.second.size() > 1){
				num += edge.second.size();
				//cout<<"Found example "<<u->x<<" with #edges of label "<<edge.first.field_name<<" equal to "<<edge.second.size()<<endl;
			}
		}
	}
	cout<<"repeating edges: "<<num<<endl;
	*/

	int num = 0;
	for(Vertex* u : vertices){
		for(auto edge : u->edges){
			num += edge.second.size();
			
		}
	}
	cout<<"edges: "<<num<<endl;
	cout<<"Vertices: "<<N<<endl;
}

/*
void graph::addedge(Vertex* u,Vertex* v,field &f){
	u->addedge(f,v->id);
	setFlattened(u->layer != 0 | v->layer != 0);

	//this is all testing code
	/*
	if(u->name.find(":") != string::npos && u->name != v->name){
		std::vector<string> tokens1;
		split(u->name,":",tokens1);

		std::vector<string> tokens2;
		split(v->name,":",tokens2);

		if(tokens1[0] == tokens2[0]){
			cout<<"adding 'unusual' edge from "<<u->name<<" to "<<v->name<<" with label "<<f.field_name<<"\\\\"<<endl;
		}
	}*/
	//if(u != v) cout<<"("<<v->name<<") -> ("<<u->name<<") with label "<<f.field_name<<endl;
	/*numedges++;
}*/

void graph::printReach(){
	cout<<"\tNumber of Strongly connected components : "<<dsu.getN()<<endl;
}

void graph::printDetailReach(){
	cout<<"Printing detailed reachability for ";
	if(isFlattened) cout<<"flattened";
	else cout<<"non-flattened";
	cout<<" graph\\\\"<<endl;

	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}

	auto it = scc.begin();
	while(it!=scc.end()){
		int zero_elems = 0;
		for(int elem : it->second){
			if(vertices[elem]->y == 0){
				zero_elems++;
				if(zero_elems>=2) break;
			}
		}
		if(zero_elems>=1){
			cout<<"scc: \\{";
			for(int elem : it->second){
				if(vertices[elem]->y == 0){ 
					cout<<"("<<vertices[elem]->x<<"), ";
					//cout<<tokens[0]<<"\n";
				}else if(true){ //TODO disable this
					cout<<"("<<vertices[elem]->x<<", "<<vertices[elem]->y<<"), ";
				}
			}
			cout<<"\\}\\\\"<<endl;
		}
		it++;
	}
}

void graph::printGraph(){
	cout<<"size of vertices is "<<vertices.size()<<endl;
	cout<<"size of fields in graph is "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){
		Vertex *vtx = vertices[i];
		cout<<"*****  "<<vtx->x<<"  *****\n";
		vtx->printvtxid();
		printEdges(vtx);
		cout<<endl;
	}
}



void graph::printEdges(Vertex *vtx){
	auto fit = vtx->edgesbegin();
	while(fit!=vtx->edgesend()){   // iterating over field
		field f = fit->first;
		cout<<"** "<<f.field_name<<endl;
		auto fedgeit = vtx->edgesbegin(f);
		while(fedgeit != vtx->edgesend(f)){   // iterating over edges
			cout<<"\t"<<vertices[*fedgeit]->x<<endl;
			fedgeit++;
		}
		fit++;
	}
}
