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

public:
	
	std::vector<Vertex*> vertices;
	DSU dsu; 
	
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

	void constructFromDot(string infile_name, bool d1_parenthesis, bool d1_bracket);
	
	graph flatten(string field_name, int depth);
	
	int calcNumReachablePairs();

	void forceRootsToLayer(int layer);
	
	//helper functions
	void setFlattened(bool v){
		isFlattened = isFlattened | v;
	}
	void iterateOverEdges(void (f)(Vertex start, Vertex end, field f, void* extra[]), void* extra[]);
	void printGraphAsTikz();
	void printAsDot();
	graph copy();

	map<int,set<int>> computeSCCs();

	bool mergeNodesBasedOnSCCsInFlattened(graph h, int height); //TODO remove

	graph copy_ignoring(string label);


	void printSparsenessFacts();

	void deleteVertices(){
		for(Vertex* v : vertices){
			delete v;
		}
	}

	//upper bound for flattening height as described by A. Pavlogiannis
	int bound(){
		return 18*N*N + 6*N;
	}

	void transplantReachabilityInformationTo(graph& other);
	map<int,set<int>> computeDisjointSets();

	//variout techniques to reduce graph sizes
	graph makeCopyWithoutDuplicates();

	//removes provably unreachable nodes
	//Based on techniques discovered 2 july 2021 and 3 july 2021
	graph trim_d1d1(graph& g_working);

	//removes provably unreachable nodes via the
	//rules applicable in the D1Dk case.
	graph trim_dkd1(graph& g_working);

	void removeHubVertexIfExistsThenCalc(graph &g_working, graph &g);

	graph buildSubgraph(set<int> &ids);

	void bidirectedInterleavedD1D1Reach();

	void bidirectedInterleavedDkD1Reach(string flatten_on);

	void printAsDot2();
};

#endif