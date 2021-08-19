#ifndef NVERTEX_H
#define NVERTEX_H

#include <unordered_map>
#include "utils/fdll.h"
#include "graph/field.h"

class Nvertex;

typedef unordered_map<field,fdll<int>> fmap;

class Nvertex{
	fmap outedges;
	fmap inedges;
public:
	int degree;
	int id;
	string name;
	int graph_num;

	Nvertex(int id,string name){
		this->id=id;
		this->name=name;
		degree=0;
		graph_num=id;
	}

	//iteration
	fmap::iterator getOutFieldbegin(){
		return outedges.begin();
	}

	fmap::iterator getOutFieldend(){
		return outedges.end();
	}

	fmap::iterator getInFieldbegin(){
		return inedges.begin();
	}

	fmap::iterator getInFieldend(){
		return inedges.end();
	}

	list<int>::iterator getOutFieldbegin(const field &f){
		return outedges[f].begin();
	}

	list<int>::iterator getOutFieldend(const field &f){
		return outedges[f].end();
	}

	list<int>::iterator getInFieldbegin(const field &f){
		return inedges[f].begin();
	}

	list<int>::iterator getInFieldend(const field &f){
		return inedges[f].end();
	}

	// addition 
	void addinEdge(const field& f,int uid){
		inedges[f].add(uid);
	}

	void addoutEdge(const field& f,int uid){
		outedges[f].add(uid);
	}

	// size and find
	int getOutsize(const field &f){
		return outedges[f].size();
	}

	int getInSize(const field &f){
		return inedges[f].size();

	}

	bool outContains(const field& f,int uid){
		return outedges[f].contains(uid);
	}

	bool inContains(const field& f,int uid){
		return inedges[f].contains(uid);
	}

	// removal
	list<int>::iterator outErase(const field& f,int uid){
		return outedges[f].erase(uid);
	}

	list<int>::iterator inErase(const field&f,int uid){
		return inedges[f].erase(uid);
	}

	void clear(){
		inedges.clear();
		outedges.clear();
	}

	void printvtxid(){
		cout<<"vertex id : "<<id<<endl;
		cout<<"different outgoing fields are "<<outedges.size()<<endl;
	}

};

namespace std{
	template <>
	struct hash<Nvertex>{
		std::size_t operator()(const Nvertex& k)const {
			return hash<int>()(k.id);
		}
	};
}


#endif