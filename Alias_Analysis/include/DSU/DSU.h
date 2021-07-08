#ifndef DSU_H
#define DSU_H

#include <vector>
using namespace std;

class DSU {
	vector<int> parent_pos;   // position of parent of vertex at position p
	vector<int> vtxatpos;
	vector<int> posofvtx;
	int N = 0;
public:
	int getN();

	void init(int n);
	// gives the root of the Disjoint set containing vertex id v 
	int root(int v);
	//merge the two Disjoined set and returns the root of the merged Disjoined set
	int merge(int a,int b);

	void hardmerge(int r,int a);
	// gives the rank of the root r
	int rank(int r);

	int rootpos(int pos);

	void exchange(int uid,int vid);

	//compute sccs from DSU data.
	map<int, set<int>> getSCCs(){
		map<int,set<int>> scc;
		for(int i=0;i<N;i++){
			scc[root(i)].insert(i);
		}
		return scc;
	}
};

#endif