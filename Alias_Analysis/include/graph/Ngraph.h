#ifndef NGRAPH_H
#define NGRAPH_H

#include "graph/Nvertex.h"
#include "graph/field.h"
#include "DSU/DSU.h"
#include "utils/utils.h"
#include "utils/fdll.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <fstream>


using namespace std;

class Ngraph{
	std::vector<Nvertex*> vertices;
	std::vector<field> fields;
	DSU dsu;
	field EPS = field(0,"eps"); // special field denoting epsilon
	fdll<pair<int,field>> worklist;
	map<string,Nvertex*> str2vtx;
	map<string,field> str2field;
	
	void initDSU();
	void moveSelfEdges(Nvertex* x,Nvertex* y,bool);
	void moveOutEdges(Nvertex* x,Nvertex* y,bool );
	void moveInEdges(Nvertex* x,Nvertex* y,bool );
	void removeVtx(Nvertex* y,bool wp);
public:
	int N;
	double numedges;
	Ngraph(){
		N=0;
		numedges=0;
		fields.push_back(EPS);
		str2field["eps"]=EPS;
	}
	void initWorklist();
	void dyck_reach();	
	void construct(string infile_name);
	void printReach();
	void printDetailReach();
	void printGraph();
	void printEdges(Nvertex* vtx);
	void addedge(Nvertex* u,Nvertex* v,field& f);
	field& getfield(const string &s);
	Nvertex* getVertex(const string &s);	
};


#endif