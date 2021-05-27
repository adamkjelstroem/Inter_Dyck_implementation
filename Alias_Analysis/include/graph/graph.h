#ifndef GRAPH_H
#define GRAPH_H

#include "graph/vertex.h"
#include "graph/field.h" 
#include "DSU/DSU.h"
#include <vector> 
#include <queue>
#include <set>
#include <string>
#include <map>
#include <fstream>

using namespace std;

class graph {

	// steps
	// g.construct(string);
	// g.initWorklist();
	// g.bidirectedReach();


	std::vector<field> fields;
	field EPS = field(0,"eps");
	
	queue<pair<int,field>> worklist;  //vertex id and field
	map<string,Vertex*> str2vtx;
	map<int, map<int,Vertex*>> pos2vtx;
	map<string,field> str2field;

	void addnewPair(Vertex *rootvtx);
	void appendedges(std::set<int>& roots,Vertex *rootvtx,int uid,field mf);
	void removeRepeatedges(list<int> &l);
	void dsuUnion(int root,std::set<int>& roots);
	string makeFlattenedName(Vertex* vertex, int layer);

public:
	
	std::vector<Vertex*> vertices;
	DSU dsu; //TODO make these private again
	
	int N = 0;
	double numedges;
	bool isFlattened = false;
	graph(){
		numedges=0;
		fields.push_back(EPS);
		str2field["eps"] = EPS;
	}
	void initWorklist();
	void bidirectedReach();
	void construct(string infile_name);

	//void addedge(Vertex *u,Vertex *v, field &f);

	//adds edge from start to end with label l. example: l="(" means   a -- "(" --> b
	void addEdge(int start_x, int start_y, int end_x, int end_y, string label);

	void printReach();
	void printDetailReach();
	void printDetailReach(DSU* dsu);
	void printGraph();
	void printEdges(Vertex *vtx);
	bool query(int uid,int vid);
	int getRoot(Vertex* vtx,field& f,std::set<int>& roots,int &rootS);
	field& getfield(const string &s);
	// gets the vertex with name, if not present creates it. name is arbitrary display value
	Vertex* getVertex(int x, int y, const string &name);

	void construct2(string infile_name);
	void construct2flattenbracket(string infile_name);
	graph flatten(string field_name, int depth);
	void flattenReach(string flatten_label);
	int calcNumReachablePairs();
	void flattenReach2(string flatten_label);
	void forceRootsToLayer(int layer);

	//helper functions
	void setFlattened(bool v){
		isFlattened = isFlattened | v;
	}
	void iterateOverEdges(void (f)(Vertex start, Vertex end, field f, void* extra[]), void* extra[]);
	void printGraphAsTikz();
	void printAsDot();
	graph copy();
};

#endif