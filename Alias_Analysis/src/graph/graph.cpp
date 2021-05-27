#include "graph/graph.h"
#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>


//same as below, but uses a different file input format.
void graph::construct2(string infile_name){
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

			//parse
			//note the 'flipping' s.t. we "add an edge" from a to b by calling in order (b, a, label)
			if (label.find("op") != string::npos){
				addEdge(a, 0, b, 0, "(");
			}else if (label.find("cp") != string::npos){
				addEdge(b, 0, a, 0, "(");
			}else if (label.find("ob") != string::npos){
				addEdge(a, 0, b, 0, "[");
			}else if (label.find("cb") != string::npos){
				addEdge(b, 0, a, 0, "[");
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

	for (int i = 0; i < depth; i++){ 
		cout<<"doing iteration "<<i<<" of "<<depth<<endl;
		void* w[] = {&g, &i, &depth, &field_name};
		iterateOverEdges(cop, w);
	}

	g.dsu.init(g.vertices.size());
	g.initWorklist();

	return g;
}

//Similar to flattenReach: 
//We flatten on 2
void graph::flattenReach2(string flatten_label){

	//construct 2 layer graph
	initWorklist(); // simplifies test code
	
	graph g = flatten(flatten_label, 2);

	bool print = false;
	
	long long n = vertices.size();
	long long c = 18*n*n + 6*n;

	if(print){
		cout<<"Doing flattenReach on '"<<flatten_label<<"'\\\\"<<endl;
		cout<<"n="<<n<<" and c="<<c<<"\\\\"<<endl;
	}
}

//forces the roots of an SCC to be in some layer, if the SCC has a member in that layer.
void graph::forceRootsToLayer(int layer){
	for(int i = 0; i < N; i++){
		if(vertices[i]->y == 0){
			if(vertices[dsu.root(i)]->y != layer){
				dsu.exchange(i, dsu.root(i));
			}
		}
	}
}

//flattens on 'flatten_label' 
void graph::flattenReach(string flatten_label) {
	
	//construct 2 layer graph
	initWorklist(); // simplifies test code
	
	graph g = flatten(flatten_label, 2);

	bool print = false;
	
	long long n = vertices.size();
	long long c = 18*n*n + 6*n;
	
	if(print){
		cout<<"Doing flattenReach on '"<<flatten_label<<"'\\\\"<<endl;
		cout<<"n="<<n<<" and c="<<c<<"\\\\"<<endl;
	}
	
	//c = 20; //TODO hardcoded
	for(long long i = 2; i < c; i++){
		cout<<"computing ... "<<(i*100/c)<<"\\% ("<<i<<" layers out of "<<c<<"). Graph size: "<<g.N<<"\\\\"<<endl;
		cout<<"Number of reachable pairs: "<<calcNumReachablePairs()<<endl;
		if(print){
			cout<<"computing ... "<<(i*100/c)<<"\\% ("<<i<<" layers out of "<<c<<"). Graph size: "<<g.N<<"\\\\"<<endl;
			cout<<"Number of reachable pairs: "<<calcNumReachablePairs()<<endl;
			cout<<"\\\\"<<endl;
			cout<<"\\\\"<<endl;
			cout<<""<<i<<" layers\\\\"<<endl;
			cout<<"starting graph for iteration / after adding new top layer:\\\\"<<endl;
			g.printGraphAsTikz();
		}

		g.initWorklist();
		//compute SCCs
		g.bidirectedReach();

		/*
		if(print){
			cout<<"DETAIL REACH\\\\"<<endl;
			g.printDetailReach();
		}*/	
		
		
		//I think 'dsu.exchange' swaps two nodes, meaning we can swap which is a root, and, if necessary, force a node to be the root.
		//so force layer zero nodes to be roots. If two layer-zero nodes are roots, keep the first one
		//we do this in g, the constructed graph
		for(int i = 0; i < g.N; i++){
			if(g.vertices[i]->y == 0){
				if(g.vertices[g.dsu.root(i)]->y != 0){
					g.dsu.exchange(i, g.dsu.root(i));
				}
			}
		}

		bool stillConnectingToNewLayer = false;
		map<int,set<int>> scc;
		for(int i=0;i<g.N;i++){
			scc[g.dsu.root(i)].insert(i);
		}


		//merge vertices in graph
		auto it = scc.begin();
		while(it!=scc.end()){
			//we only care about sccs where the root has y=0
			if(g.vertices[it->first]->y == 0){
				
				int root_id_in_this=getVertex(g.vertices[it->first]->x, 0, "")->id;
				for(int elem : it->second){
					if(g.vertices[elem]->y == 0){
						//Hardmerge(root, a) forces first parameter to be root. TODO is this necessary?
						dsu.merge(
							root_id_in_this,
							getVertex(g.vertices[elem]->x, 0, "")->id
							);
					}else if(g.vertices[elem]->y == i-1){
						//cout<<g.vertices[it->first]->to_string()<<" is connecting to "<<g.vertices[elem]->to_string()<<endl;
						stillConnectingToNewLayer = true;
					}
				}
			}
			it++;
		}
		if(!stillConnectingToNewLayer){
			if(print){
				cout<<"there is no connection between the bottom layer and the new top layer, so no further flattening gives additional information. Terminating.\\\\"<<endl;
			}
			break;
		}

		//build new graph
		//cout<<"building reduced graph"<<endl;
		graph g2;


		auto cop = [](Vertex a, Vertex b, field f, void* extra[]) {
			auto g =  (graph*)extra[0];
			auto g2 = (graph*)extra[1];


			auto start_root = g->vertices[g->dsu.root(a.id)];
			auto end_root =   g->vertices[g->dsu.root(b.id)];
			

			//cout<<"adding edge from "<<a.to_string()<<" to "<<b.to_string()<<". when converted to roots, we add edge from "<<start_root->to_string()<<" to "<<end_root->to_string()<<"\\\\"<<endl;
			
			g2->addEdge(
				start_root->x, start_root->y, 
				end_root->x,end_root->y, 
				f.field_name);
			
		};

		/*
		if(print){
			map<int,set<int>> scc2;
			for(int i=0;i<g.N;i++){
				scc2[g.dsu.root(i)].insert(i);
			}
			auto it = scc2.begin();
			while(it!=scc2.end()){
				int zero_elems = 0;
				cout<<"scc with root "<<g.vertices[it->first]->to_string()<<": ";
				for(int elem : it->second){
					cout<<g.vertices[elem]->to_string()<<",";
				}
				cout<<"\\\\"<<endl;
				it++;
			}
		}*/

		void* w[] = {&g, &g2};

		g.iterateOverEdges(cop, w);

		if(print){
			cout<<"reduced graph:\\\\"<<endl;
			g2.printGraphAsTikz();
		}	

		
		//Keep this code around as backup:
		if(true){
			auto addL = [](Vertex a, Vertex b, field f, void* extra[]) {
				graph* current = (graph*)extra[0];
				graph* next_iteration = (graph*)extra[1];
				int i = *((int*)extra[2]);
				string field_name = *(string*)extra[3];

				if(f.field_name == field_name){ //TODO cache id of "[" field and do comparison on
					//add 'epsilon' edge from node-set in lower layers to node in new layer
					//we don't care about order, as epsilon edges are undirected.

					//a is vertex in original graph

					auto a_in_layer_i_minus_1_in_current =  current->getVertex(a.x, i-1, ""); //TOD name here??

					auto root_of_a_in_current = current->vertices[current->dsu.root(a_in_layer_i_minus_1_in_current->id)]; 

					//add new edge in g2
					next_iteration->addEdge(
						root_of_a_in_current->x, root_of_a_in_current->y,
						b.x, i,
						next_iteration->EPS.field_name
					);
				}else{
					//add edges that go inside new layer
					next_iteration->addEdge(
						a.x, i,
						b.x, i,
						f.field_name
					);
				}
			};

			void* q[] = {&g, &g2, &i, &flatten_label};

			iterateOverEdges(addL, q);
		}

		
		if(false){ //TODO does not work
			auto addL2 = [](Vertex a, Vertex b, field f, void* extra[]) {
			graph* original = (graph*)extra[0];
			graph* current = (graph*)extra[1];
			graph* next_iteration = (graph*)extra[2];
			int i = *((int*)extra[3]);
			string field_name = *(string*)extra[4];

			//cout<<"adding edge from "<<a.to_string()<<" to "<<b.to_string()<<" with label "<<f.field_name<<endl;

			//a is vertex in original graph
			auto root_of_b_in_original = original->vertices[original->dsu.root(b.id)]; 


			if(f.field_name == field_name){ //TODO cache id of "[" field and do comparison on
				
				//add 'epsilon' edge from node-set in lower layers to node in new layer
				//we don't care about order, as epsilon edges are undirected.


				//auto a_in_layer_i_minus_1_in_current = current->getVertex(a.x, i-1, "");
				//auto root_of_a_in_current = current->vertices[current->dsu.root(a_in_layer_i_minus_1_in_current->id)];

				auto a_in_original = original->getVertex(a.x, 0, "");
				auto root_of_a_in_original = original->vertices[original->dsu.root(a_in_original->id)];


				auto a_in_layer_i_minus_1_in_current = current->getVertex(root_of_a_in_original->x, i-1, "");
				auto root_of_a_in_current = current->vertices[current->dsu.root(a_in_layer_i_minus_1_in_current->id)];

				next_iteration->addEdge(
					root_of_a_in_current->x, root_of_a_in_current->y,
					root_of_b_in_original->x, i,
					next_iteration->EPS.field_name
				);
			}else{
				//b is vertex in new graph
				auto root_of_a_in_original = original->vertices[original->dsu.root(a.id)];

				//add edges that go inside new layer
				next_iteration->addEdge(
					root_of_a_in_original->x, i,
					root_of_b_in_original->x, i,
					f.field_name
				);
			}
		};

		void* q[] = {this, &g, &g2, &i, &flatten_label};

		iterateOverEdges(addL2, q);
		}


		//before using new graph:
		g2.dsu.init(g2.vertices.size());

		//overwrite old graph with new
		g = g2; //TODO maybe memory leak


		//update n and c based on graph shrinkage
		n = 0;
		set<int> scc3;
		for(int i=0;i<this->N;i++){
			scc3.insert(dsu.root(i));
		}	
		n = scc3.size();
		
		if(n == 1){
			if(print)
				cout<<"All vertices are in the same SCC, so no more information can be discovered. Terminating."<<endl;
			break;
		} 

		c = 18*n*n + 6*n;

		if(print)
			cout<<"c has been updated to "<<c<<"\\\\"<<endl;
	}


	//g.printGraphAsTikz();

	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}	
	cout<<"Number of sccs: "<<scc.size()<<endl;
	

	//Print results
	/*
	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}	
	auto it = scc.begin();
	while(it!=scc.end()){
		cout<<"scc: \\{";
		for(int elem : it->second){
			cout<<vertices[elem]->name<<", "; 
		}
		cout<<"\\}\\\\"<<endl;
		
		it++;
	}*/
}


void graph::iterateOverEdges(void (f)(Vertex start, Vertex end, field f, void* extra[]), void* extra[]){
	for(int j=0;j<N;j++){
		auto vertex = vertices[j];
		auto fit = vertex->edgesbegin();

		while(fit!=vertex->edgesend()){   // iterating over field
			field fi = fit->first;
			//f.field_name is edge label name
			auto fedgeit = vertex->edgesbegin(fi);
			while(fedgeit != vertex->edgesend(fi)){   // iterating over edges
				
				(*f)(*vertices[*fedgeit], *vertices[j], fi, extra);
				
				fedgeit++;
			}
			fit++;
		}
	}
}

graph graph::copy(){
	graph g;

	auto cop = [](Vertex a, Vertex b, field f, void* extra[]) {
		graph* g = (graph*)extra[0];
		g->addEdge(a.x, a.y, b.x, b.y, f.field_name);
	};
	void* w[] = {&g};
	iterateOverEdges(cop, w);

	g.dsu.init(g.vertices.size());

	return g;
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
			//cout<<"analyzing element "<<vertices[elem]->id <<" with layer "<<vertices[elem]->layer<<endl;
		}
		n += zero_elems * (zero_elems-1) / 2;
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
				}else{
					cout<< "\\path [->, red] ("<<b.id<<") edge [bend right=20] node {$ $} ("<<a.id<<");"<<endl;
				}
			}
		};

		iterateOverEdges(print, nullptr);
	}

	cout<<"\\end{tikzpicture}\\\\"<<endl;

}

//prints the graph in the 'dot' format
void graph::printAsDot(){
	iterateOverEdges(
		[](Vertex a, Vertex b, field f, void* extra[]) {
			cout<<a.name<<"->"<<b.name<<"[label=\""<<f.field_name<<endl;
		}, 
	nullptr);
}



//this also does flattening on one dyck language
//TODO old code, should be removed?
/*
void graph::construct2flattenbracket(string infile_name){
	//TODO old code; should be removed.
	ifstream infile(infile_name);
	string line;

	int n = 5; //TODO hardcoded for now
	int c = n*n*12+n*6; //expansion factor.
	//c = 1;

	while(std::getline(infile, line)){
		// if this is a non-comment line
		if(line.find("//") != string::npos){
			continue; //skip commented lines
		}
		if(line.find("->") != string::npos){
			std::vector<string> tokens;
			split(line, "->", tokens);
			string a = tokens[0];
			std::vector<string> tokens2;
			split(tokens[1], "[label=\"", tokens2);
			string b = tokens2[0];
			string label = tokens2[1];

			//parse
			//cout<<"adding edge from "<<a<<" to "<<b<<" with label= "<<label<<endl;
			if (label.find("op") != string::npos){
				//we're flattening on brackets, so parentesis edges are mostly left as-is
				for(int i = 0; i <= c; i++){ //note the <= operator
					addEdge(a, i, b, i), getfield("("));
				}
			}else if (label.find("cp") != string::npos){
				for(int i = 0; i <= c; i++){ //note the <= operator
					//reverse order of a and b because of closing parentheses
					addedge(getVertex(b, i), getVertex(a, i), getfield("("));
				}
			}else if (label.find("ob") != string::npos){
				for(int i = 0; i < c; i++){ //note the < operator
					addedge(getVertex(a, i+1), getVertex(a, i), EPS);
				}
			}else if(label.find("cb") != string::npos){
				for(int i = 0; i < c; i++){ //note the < operator
					addedge(getVertex(b, i), getVertex(a, i+1), EPS);
				}
			}else{
				for(int i = 0; i <= c; i++){ //note the <= operator
					addedge(getVertex(b, i), getVertex(a, i), EPS);
				}
			}
		}
	}
	dsu.init(vertices.size());
}*/

// takes the file name containing the edge information of the spg as arguement
// and construct a Ngraph from it
void graph::construct(string infile_name){
	/*
	ifstream infile(infile_name);
	string line;
	while(std::getline(infile,line)){
		std::vector<string> tokens;
		split(line,"||",tokens);
		if(tokens.size()==0 || (tokens.size()==1 && tokens[0].size()==0))
			continue;
		if(tokens[0] == "e"){
			// assert(tokens.size()==4);
			//Note the flipping of tokens[1] and tokens[2]
			addEdge(tokens[2], 0,tokens[1], 0,tokens[3]);
			// if(getfield(tokens[3])==EPS){
			// 	addedge(getVertex(tokens[2]),getVertex(tokens[1]),EPS);
			// }
			continue;
		}
		if(tokens[0] == "v"){
			// assert(tokens.size()==2);
			getVertex(tokens[1], 0);
			continue;
		}
		if(tokens[0] == "f"){
			// assert(tokens.size()==2);
			getfield(tokens[1]);
			continue;
		}
		if(tokens[0] == "//"){
			// comment in infile
			continue;
		}
		cerr<<"incorrect syntax in "+infile_name<<endl;
		cerr<<line<<endl;
		cerr<<"tokens were "<<tokens.size()<<endl;
		for(int i=0;i<tokens.size();i++)
			cerr<<tokens[i]<<"   **;**   ";
		cerr<<endl;
	}
	dsu.init(vertices.size());*/ //TODO legacy code for old format, should be revisited
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
			if(it->second.size()>=2)
				worklist.push(pair<int,field>(vtx->id,it->first));
			it++;
		}
	}
}

// algorithm described in this paper
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
				if(vertices[elem]->y == 0){ //TODO disabled for test
					cout<<"("<<vertices[elem]->x<<"), ";
					//cout<<tokens[0]<<"\n";
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
		cout<<"*****  "<<vtx->name<<"  *****\n";
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
			cout<<"\t"<<vertices[*fedgeit]->name<<endl;
			fedgeit++;
		}
		fit++;
	}
}
