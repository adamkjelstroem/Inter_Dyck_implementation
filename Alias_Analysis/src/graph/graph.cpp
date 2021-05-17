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
			string a = tokens[0];
			std::vector<string> tokens2;
			split(tokens[1], "[label=\"", tokens2);
			string b = tokens2[0];
			string label = tokens2[1];

			//parse
			//note the 'flipping' s.t. we "add an edge" from a to b by calling in order (b, a, label)
			if (label.find("op") != string::npos){
				addedge(getVertex(b), getVertex(a), getfield("("));
			}else if (label.find("cp") != string::npos){
				addedge(getVertex(a), getVertex(b), getfield("("));
			}else if (label.find("ob") != string::npos){
				addedge(getVertex(b), getVertex(a), getfield("["));
			}else if (label.find("cb") != string::npos){
				addedge(getVertex(a), getVertex(b), getfield("["));
			}else{
				addedge(getVertex(b), getVertex(a), EPS);
			}
			//cout<<"adding edge from "<<a<<" to "<<b<<" with label= "<<label<<endl;
		}
	}
	dsu.init(vertices.size());
}

//performs graph flattening on brackets to depth 'depth'
graph graph::flattenbracket(int depth){
	graph g;

	for (int i = 0; i < depth; i++){ 
		for (auto vertex : vertices){
			auto fit = vertex->edgesbegin();
			while(fit!=vertex->edgesend()){   // iterating over field
				field f = fit->first;
				//f.field_name is edge label name
				auto fedgeit = vertex->edgesbegin(f);
				while(fedgeit != vertex->edgesend(f)){   // iterating over edges
					//"vertices[*fedgeit]" is end vertex
					//"vertex" is start vertex

					if(f.field_name == "["){ //TODO cache id of "[" field and do comparison on
						//flatten on brackets
						if(i+1!=depth)
							g.addedge(
								g.getVertex(vertex->name + ": " + to_string(i+1)),
								g.getVertex(vertices[*fedgeit]->name + ": " + to_string(i)), //end node
								g.EPS
							);
					}else{
						g.addedge(
							g.getVertex(vertex->name + ": " + to_string(i)),
							g.getVertex(vertices[*fedgeit]->name + ": " + to_string(i)), //end node
							g.getfield(f.field_name)
						);
					}

					fedgeit++;
				}
				fit++;
			}
		}
	}
	g.dsu.init(g.vertices.size());

	return g;
}


void graph::flattenReach() {
	//construct 2 layer graph
	initWorklist(); //TODO is this necessary?
	graph g = flattenbracket(2);

	
	
	int n = vertices.size();
	int c = 18*n*n + 6*n;
	c = 6; //TODO hardcoded
	for(int i = 2; i < c; i++){
		//demo
		g.initWorklist();
		
		//cout<<"\\\\"<<endl;
		//cout<<"\\\\"<<endl;
		//cout<<""<<i<<" layers\\\\"<<endl;

		//g.printFlattenedGraphAsTikz();


		//compute SCCs
		g.bidirectedReach();

		//g.printDetaiLReachInterDyck();

		//find layer zero items that have been joined, and merge them
		
		
		map<int,set<int>> scc;
		for(int i=0;i<g.N;i++){
			scc[g.dsu.root(i)].insert(i);
		}
		auto it = scc.begin();
		while(it!=scc.end()){
			int zero_elems = 0;
			int first_zero = -1;
			for(int elem : it->second){
				std::vector<string> tokens;
				split(g.vertices[elem]->name,":",tokens);
				if(tokens[1] == "0"){
					zero_elems++;
					first_zero = std::stoi(tokens[0]);
					if(zero_elems>=2) break;
				}
			}
			if(zero_elems>=2){
				for(int elem : it->second){
					std::vector<string> tokens;
					split(g.vertices[elem]->name,":",tokens);
					if(tokens[1] == "0"){ //TODO disabled for test
						dsu.merge(
							dsu.root(std::stoi(tokens[0])),
							dsu.root(first_zero)
							);
					}
				}
			}
			it++;
		}

		//build new graph
		graph g2;
		for(int j=0;j<g.N;j++){
			auto vertex = g.vertices[j];
			auto fit = vertex->edgesbegin();

			auto start_root = g.vertices[g.dsu.root(j)]->name;
			while(fit!=vertex->edgesend()){   // iterating over field
				field f = fit->first;
				//f.field_name is edge label name
				auto fedgeit = vertex->edgesbegin(f);
				while(fedgeit != vertex->edgesend(f)){   // iterating over edges
					auto end_root = g.vertices[g.dsu.root(*fedgeit)]->name;
			
					
					//"vertices[*fedgeit]" is end vertex
					//"vertex" is start vertex
					
					//cout<<"adding edge from "<<start_root<<" to "<<end_root<<" with field "<<f.field_name<<endl;

					g2.addedge(
						g2.getVertex(start_root),
						g2.getVertex(end_root), //end node
						g2.getfield(f.field_name)
					);
					
					fedgeit++;
				}
				fit++;
			}
		}
		
		//add new layer
		for (int k = 0; k < vertices.size(); k++){
			auto vertex = vertices[k];
			auto fit = vertex->edgesbegin();
			while(fit!=vertex->edgesend()){   // iterating over field
				field f = fit->first;
				//f.field_name is edge label name
				auto fedgeit = vertex->edgesbegin(f);
				while(fedgeit != vertex->edgesend(f)){   // iterating over edges
					//"vertices[*fedgeit]" is end vertex
					//"vertex" is start vertex
					if(f.field_name == "["){ //TODO cache id of "[" field and do comparison on
						//flatten on brackets
						//if(i+1!=c)
						
						auto start_name = vertices[*fedgeit]->name + ": " + to_string(i-1);

						auto start_root = g.vertices[g.dsu.root(g.getVertex(start_name)->id)]->name;
						
						//auto end_root = g.vertices[g.dsu.root(*fedgeit)]->name;
						//auto start_root = g.vertices[g.dsu.root(k)]->name;
						
						//if(*fedgeit == k) break;
						
						//cout<<"adding edge from "<<start_name<<" to "<<(vertex->name + ": " + to_string(i))<<" with label "<<f.field_name<<"\\\\"<<endl;
						//cout<<"\\\\"<<endl;

					
						//flattened edges connect between layers
						g2.addedge(
							g2.getVertex(vertex->name + ": " + to_string(i)),
							g2.getVertex(start_root), //end node
							g2.EPS
						);
					}else{
						//non-flattened edges connect 'inside' layers
						g2.addedge(
							g2.getVertex(vertex->name + ": " + to_string(i)),
							g2.getVertex(vertices[*fedgeit]->name + ": " + to_string(i)), //end node
							g2.getfield(f.field_name)
						);
					}

					fedgeit++;
				}
				fit++;
			}
		}

		//before using new graph:
		g2.dsu.init(g2.vertices.size());


		//overwrite old graph with new
		g = g2;
	}


	//g.printFlattenedGraphAsTikz();

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
			cout<<vertices[elem]->id<<", "; //NOTE using id, not name!
		}
		cout<<"\\}\\\\"<<endl;
		
		it++;
	}
	*/
}

void graph::printNumReachablePairs(){
	int n = 0;
	//Print results
	map<int,int> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)]++;
	}	
	auto it = scc.begin();
	while(it!=scc.end()){
		n += (it->second - 1) * it->second / 2;	
		it++;
	}
	cout<<"Number of reachable pairs:"<<endl;
	cout<<n<<endl;
}

void graph::printFlattenedGraphAsTikz(){

	cout<<"\\begin{tikzpicture}"<<endl;

	for (Vertex* v : vertices){
		std::vector<string> tokens;
		split((*v).name ,":",tokens);
		
		cout<<"\\node ("<<(*v).id<<") at ("<<std::stoi(tokens[0]) * 2<<","<<std::stoi(tokens[tokens.size() - 1])*2<<") {"<<(*v).name<<"};"<<endl;

	}

	cout<<endl;

	for (Vertex* vtx : vertices){
		auto fit = vtx->edgesbegin();
		while(fit!=vtx->edgesend()){   // iterating over field
			field f = fit->first;
			auto fedgeit = vtx->edgesbegin(f);
			while(fedgeit != vtx->edgesend(f)){   // iterating over edges
				if(vtx->id != vertices[*fedgeit]->id){
					if (f.field_name == "eps"){
						cout<< "\\path ("<<vertices[*fedgeit]->id<<") edge node {$ $} ("<<vtx->id<<");"<<endl;
					}else{
						cout<< "\\path [->, red] ("<<vertices[*fedgeit]->id<<") edge [bend right=20] node {$ $} ("<<vtx->id<<");"<<endl;
					}
				}
				fedgeit++;
			}
			fit++;
		}
	}

	//for (nodev)

	cout<<"\\end{tikzpicture}\\"<<endl;

}


//this also does flattening on one dyck language
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
			//note that a and b are flipped. this makes the graph compatible with the rest of the code base
			//quite bizarre.
			if (label.find("op") != string::npos){
				//we're flattening on brackets, so parentesis edges are mostly left as-is
				for(int i = 0; i <= c; i++){ //note the <= operator
					addedge(getVertex(b + ": " + std::to_string(i)), getVertex(a + ": " + std::to_string(i)), getfield("("));
				}
			}else if (label.find("cp") != string::npos){
				for(int i = 0; i <= c; i++){ //note the <= operator
					//reverse order of a and b because of closing parentheses
					addedge(getVertex(a + ": " + std::to_string(i)), getVertex(b + ": " + std::to_string(i)), getfield("("));
				}
			}else if (label.find("ob") != string::npos){
				for(int i = 0; i < c; i++){ //note the < operator
					addedge(getVertex(b + ": " + std::to_string(i+1)), getVertex(a + ": " + std::to_string(i)), EPS);
				}
			}else if(label.find("cb") != string::npos){
				for(int i = 0; i < c; i++){ //note the < operator
					addedge(getVertex(b + ": " + std::to_string(i)), getVertex(a+ ": " + std::to_string(i+1)), EPS);
				}
			}else{
				for(int i = 0; i <= c; i++){ //note the <= operator
					addedge(getVertex(b + ": " + std::to_string(i)), getVertex(a + ": " + std::to_string(i)),EPS);
				}
			}
		}
	}
	dsu.init(vertices.size());
}

// takes the file name containing the edge information of the spg as arguement
// and construct a Ngraph from it
void graph::construct(string infile_name){
	ifstream infile(infile_name);
	string line;
	while(std::getline(infile,line)){
		std::vector<string> tokens;
		split(line,"||",tokens);
		if(tokens.size()==0 || (tokens.size()==1 && tokens[0].size()==0))
			continue;
		if(tokens[0] == "e"){
			// assert(tokens.size()==4);
			addedge(getVertex(tokens[1]),getVertex(tokens[2]),getfield(tokens[3]));
			// if(getfield(tokens[3])==EPS){
			// 	addedge(getVertex(tokens[2]),getVertex(tokens[1]),EPS);
			// }
			continue;
		}
		if(tokens[0] == "v"){
			// assert(tokens.size()==2);
			getVertex(tokens[1]);
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
	dsu.init(vertices.size());
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



Vertex* graph::getVertex(const string &s){
	auto it = str2vtx.find(s);
	if(it==str2vtx.end()){
		Vertex* vtx = new Vertex(this->N,s);
		vertices.push_back(vtx);
		vtx->addedge(EPS,vtx->id);
		this->N++;
		str2vtx[s]=vtx;
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

void graph::addedge(Vertex* u,Vertex* v,field &f){
	u->addedge(f,v->id);
	//if(u != v) cout<<"("<<v->name<<") -> ("<<u->name<<") with label "<<f.field_name<<endl;
	numedges++;
}

void graph::printReach(){
	cout<<"\tNumber of Strongly connected components : "<<dsu.getN()<<endl;
}

void graph::printDetailReach(){
	cout<<"Number of Strongly connected components : "<<dsu.getN()<<endl;
	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}
	auto it = scc.begin();
	while(it!=scc.end()){
			cout<<"scc "<<it->first<<": \\{";
		for(int elem : it->second){
			cout<<vertices[elem]->name<<", ";
			// cout<<vertices[elem]->id<<" ";
		}
		cout<<"\\}\\\\"<<endl;
		it++;
	}
}

void graph::printDetaiLReachInterDyck(){
	map<int,set<int>> scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}
	auto it = scc.begin();
	while(it!=scc.end()){
		int zero_elems = 0;
		for(int elem : it->second){
			std::vector<string> tokens;
			split(vertices[elem]->name,":",tokens);
			if(tokens[1] == "0"){
				zero_elems++;
				if(zero_elems>=2) break;
			}
		}
		if(zero_elems>=1){
			cout<<"scc: \\{";
			for(int elem : it->second){
				std::vector<string> tokens;
				split(vertices[elem]->name,":",tokens);
				if(tokens[1] == "0"){ //TODO disabled for test
					cout<<tokens[0]<<", ";
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
