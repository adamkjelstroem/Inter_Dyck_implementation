#include "graph/Ngraph.h"
#include <algorithm>
#include <cassert>


// the algorithm as described in 
// Fast Algorithms for Dyck-CFL-Reachability with Applications to Alias Analysis paper
void Ngraph::dyck_reach(){
	// precond: initWorklist() has been called;
	while(worklist.size()!=0){
		pair<int,field> p = worklist.front();
		// cout<<vertices[p.first]->name<<endl;
		Nvertex* z = vertices[p.first];
		// assert(z->getOutsize(p.second)>1);

		auto it = z->getOutFieldbegin(p.second);
		Nvertex* x = vertices[*it];
		it++;
		Nvertex* y = vertices[*it];
		// assert(x->id!=y->id);
		if(x->degree < y->degree){
			Nvertex* temp = x;
			x = y;
			y = temp;
		}
		x->degree += y->degree;
		dsu.hardmerge(dsu.root(x->id),dsu.root(y->id));
		// cout<<"moving outgoing edges "<<endl;
		moveOutEdges(x,y,true);
		// cout<<"moving incoming edges "<<endl;
		moveInEdges(x,y,true);
		// cout<<"removing  edge "<<endl;
		removeVtx(y,true);
	}
}


void Ngraph::initWorklist(){
	// precond: construct(file_name) has been called 
	// cout<<"Number of vertices : "<<vertices.size()<<endl;
	// cout<<"Number of edges : "<<numedges<<endl;
	// cout<<"Number of field types : "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){
		if(i!=dsu.root(i))
			continue;
		Nvertex* vtx = vertices[i];
		auto it = vtx->getOutFieldbegin();
		while(it!=vtx->getOutFieldend()){
			field f = it->first;
			if(vtx->getOutsize(f)>=2){
				worklist.add(make_pair(vtx->id,f));
			}
			it++;
		}
	}
}

void Ngraph::initDSU(){
	dsu.init(vertices.size());
}

void Ngraph::moveSelfEdges(Nvertex* x,Nvertex* y,bool wp){
	auto it = y->getOutFieldbegin();
	while(it!=y->getOutFieldend()){
		field f = it->first;
		if(y->outContains(f,y->id)){
			if(!x->outContains(f,x->id)){
				x->addoutEdge(f,x->id);
				auto p = make_pair(x->id,f);
				if(x->getOutsize(f)>1 and !worklist.contains(p) and wp){
					worklist.add(p);
				}
			}
			y->outErase(f,y->id);
			// remove y_f from worklist
		}
		it++;
	}
}

void Ngraph::moveOutEdges(Nvertex* x,Nvertex* y,bool wp){
	// cout<<"moving self edges"<<endl;
	moveSelfEdges(x,y,wp);
	auto fit = y->getOutFieldbegin();
	while(fit!=y->getOutFieldend()){
		field f = fit->first;
		auto eit = y->getOutFieldbegin(f);
		while(eit!=y->getOutFieldend(f)){
			if(!x->outContains(f,*eit)){
				x->addoutEdge(f,*eit);
				if(*eit!=x->id)
					vertices[*eit]->addinEdge(f,x->id);
				auto p = make_pair(x->id,f);
				if(x->getOutsize(f)>1 and !worklist.contains(p) and wp){
					worklist.add(p);
				}
			}
			vertices[*eit]->inErase(f,y->id);
			eit = y->outErase(f,*eit); //iterator
			// remove y_f from worklist
		}
		fit++;
	}
}

void Ngraph::moveInEdges(Nvertex* x,Nvertex* y,bool wp){
	auto fit = y->getInFieldbegin();
	while(fit!=y->getInFieldend()){
		field f = fit->first;
		auto eit = y->getInFieldbegin(f);
		while(eit!=y->getInFieldend(f)){
			Nvertex *w = vertices[*eit];
			if(!x->inContains(f,*eit)){
				if(x!=w)
					x->addinEdge(f,*eit);
				w->addoutEdge(f,x->id);
			}
			w->outErase(f,y->id);
			eit=y->inErase(f,*eit);
			auto p = make_pair(w->id,f);
			if(w->getOutsize(f)<2 and worklist.contains(p) and wp){
				worklist.erase(p);
			}
		}
		fit++;
	}
}

void Ngraph::removeVtx(Nvertex *y,bool wp){
	auto fit = y->getOutFieldbegin();
	while(fit!=y->getOutFieldend()){
		field f =fit->first;
		auto p = make_pair(y->id,f);
		if(worklist.contains(p) and wp){
			worklist.erase(p);
		}
		fit++;
	}
	y->clear();
	N--;
}


// takes the file name containing the edge information of the spg as arguement 
// and construct a Ngraph from it 
void Ngraph::construct(string infile_name){
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
			if(getfield(tokens[3])==EPS){
				addedge(getVertex(tokens[2]),getVertex(tokens[1]),EPS);	
			}
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
	initDSU();
}


Nvertex* Ngraph::getVertex(const string &s){
	auto it = str2vtx.find(s);
	if(it==str2vtx.end()){
		Nvertex* vtx = new Nvertex(this->N,s);
		vertices.push_back(vtx);
		vtx->addoutEdge(EPS,vtx->id);
		this->N++;
		str2vtx[s]=vtx;
		return vtx;
	}
	return it->second; 
}

field& Ngraph::getfield(const string &s){
	auto it = str2field.find(s);
	if(it==str2field.end()){
		field f(fields.size(),s);
		fields.push_back(f);
		str2field[s]=fields[fields.size()-1];
		return fields[fields.size()-1];
	}
	return it->second;
}

void Ngraph::addedge(Nvertex* u,Nvertex* v,field &f){
	u->addoutEdge(f,v->id);
	u->degree++;
	if(u->id!=v->id){
		v->addinEdge(f,u->id);
		v->degree++;
	}
	if(f==EPS) // u to v eps edge implicitly implies a v to u eps edge.  
		numedges+=0.5;  // both edges are added to the graph. only one edge is counted
	else
		numedges++;	
}


void Ngraph::printReach(){
	cout<<"\tNumber of Strongly connected components : "<<dsu.getN()<<endl;
}

void Ngraph::printDetailReach(){
	cout<<"Number of Strongly connected components : "<<dsu.getN()<<endl;
	map<int,set<int> > scc;
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}
	auto it = scc.begin();
	while(it!=scc.end()){
		cout<<"printing elements belonging to scc "<<it->first<<endl;
		for(int elem : it->second){
			cout<<vertices[elem]->name<<"   ";
			// cout<<vertices[elem]->id<<" ";
		}
		cout<<endl;
		it++;
	}
}

void Ngraph::printGraph(){
	cout<<"size of vertices is "<<vertices.size()<<endl;
	cout<<"size of fields in graph is "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){
		Nvertex *vtx = vertices[i];
		cout<<"*****  "<<vtx->name<<"  *****\n";
		vtx->printvtxid();
		printEdges(vtx);
		cout<<endl;
	}
}

void Ngraph::printEdges(Nvertex* vtx){
	auto fit = vtx->getOutFieldbegin();  // iterating over field
	while(fit!=vtx->getOutFieldend()){
		field f = fit->first; 
		cout<<"** "<<f.field_name<<endl;
		auto fedgeit = vtx->getOutFieldbegin(f);
		while(fedgeit != vtx->getOutFieldend(f)){  // iterating over edges in the field
			cout<<"\t"<<vertices[*fedgeit]->name<<endl;
			fedgeit++;
		}
		fit++;
	}
}